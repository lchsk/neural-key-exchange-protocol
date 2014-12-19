#include "control.h"
#include "config.h"
#include "definitions.h"
#include "protocol.h"
#include "util.h"
#include <vector>
#include <fstream>

using namespace std;

control_t::control_t(config_t* c) : config(c)
{
    tree1 = new tree_t(config->get_perceptrons(), config->get_inputs(), config->get_range());
    tree2 = new tree_t(config->get_perceptrons(), config->get_inputs(), config->get_range());
    trees = NULL;
    trace_filename = "trace.txt";
    weights_filename = "weights.txt";
}

control_t::~control_t()
{   
    delete tree1;
    delete tree2;
    delete[] trees;
}

bool 
control_t::check_protocol_synchronisation()
{
    if (tree1->is_synchronised(tree2))
    {
        printf("Weights of trees 1 and 2:\n");
        tree1->print_weights();
        tree2->print_weights();

        return true;
    }

    return false;
}

bool 
control_t::check_attack_success()
{
    for (int a = 0; a < config->get_attackers(); a++)
    {
        if (tree1->is_synchronised(trees[a]))
        {
            printf("Tree no. %d synchronised.\n", a);
            printf("Weights of tree 1:\n");
            tree1->print_weights();
            printf("Weights of tree no. %d:\n", a);
            trees[a]->print_weights();
            return true;
        }
    }

    return false;
}

bool
control_t::run()
{
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &config->nodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &config->rank);

    double start = get_time();

    if (config->rank == MASTER)
        config->print_parameters();

    if (config->get_type() == run_type_t::PROTOCOL_ONLY 
        || 
        config->get_type() == run_type_t::ONLINE_ATTACK_SERIAL)
    {
        if (config->rank == MASTER)
            run_serial();
    }
    else if (config->get_type() == run_type_t::ONLINE_ATTACK_MPI)
        run_parallel();
    else if (config->get_type() == run_type_t::OFFLINE_ATTACK)
    {
        load_offline_data();
        run_offline();
    }

    print_time(get_difference(get_time(), start));
    MPI_Finalize();
}

void 
control_t::print_time(double p_time)
{
    printf("Time: %*.*f [ms]\n\n", RESULT_LENGTH, FLOAT_PRECISION, p_time);
}

bool
control_t::run_parallel()
{
    int attackers_per_proc = config->get_attackers() / config->nodes;
    trees = new tree_t*[config->get_attackers()];

    for (int a = 0; a < config->get_attackers(); a++)
        trees[a] = new tree_t(config->get_perceptrons(), config->get_inputs(), config->get_range());        

    for (int s = 0; s < config->get_iterations(); s++)
    {
        vector<int> input = random_vector(config->get_all_inputs());

        if (s == 0)
        {
            tree1->evaluate(input, true, true);
            tree2->evaluate(input, true, true);

            for (int a = 0; a < config->get_attackers(); a++)
                trees[a]->evaluate(input, true, true);
        }

        srand(get_random_seed());

        if (tree1->evaluate(input, true, false) == tree2->evaluate(input, true, false))
        {
            tree1->update_weights();
            tree2->update_weights();

            for (int i = 0, a = config->rank * attackers_per_proc; i < attackers_per_proc; i++, a++)
            {
                if (tree1->evaluate(input, true, false) == trees[a]->evaluate(input, true, false))
                {
                    trees[a]->update_weights();
                }
                else
                {   
                    trees[a]->find_min();
                    trees[a]->update_weights();
                }
            }
        }

        if (check_attack_success())
        {
            sent_kill_order(config->rank);
            printf("Attack was successful.\n");

            return true;
        }

        MPI_Status sta;
        int flag;

        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &sta);
        if (flag)
        {
            if (sta.MPI_TAG == TAG_KILL)
            {
                printf("Process #%d was ordered to be killed by #%d.\n", config->rank, sta.MPI_SOURCE);
                return true;
            }
        }

        if (config->rank == MASTER && check_protocol_synchronisation())
        {
            sent_kill_order(config->rank);
            printf("Trees 1 and 2 synchronised.\n");
            return true;
        }
    }

    return false;
}

bool 
control_t::run_offline()
{
    int count = 0;
    int block_size = get_block_size();
    int blocks = offline_data.size() / block_size;

    int attackers_per_proc = config->get_attackers() / config->nodes;
    trees = new tree_t*[config->get_attackers()];

    for (int a = 0; a < config->get_attackers(); a++)
        trees[a] = new tree_t(config->get_perceptrons(), config->get_inputs(), config->get_range());

    for (int s = 0; s < blocks; s++)
    {
        //printf("%d/%d\n", s, blocks);

        block_t block;
        std::vector<int> line = get_block(offline_data, s * block_size, block_size);
        fill_block(line, block);

        if (s == 0)
        {
            for (int a = 0; a < config->get_attackers(); a++)
                trees[a]->evaluate(block.inputs, true, true);
        }

        srand(get_random_seed());

        if (block.tree1_eval == block.tree2_eval)
        {
            for (int i = 0, a = config->rank * attackers_per_proc; i < attackers_per_proc; i++, a++)
            {
                if (block.tree1_eval == trees[a]->evaluate(block.inputs, true, false))
                {
                    trees[a]->update_weights();
                }
                else
                {   
                    trees[a]->find_min();
                    trees[a]->update_weights();
                }
            }
        }

        for (int i = 0, a = config->rank * attackers_per_proc; i < attackers_per_proc; i++, a++)
        {
            std::vector<int> w = trees[a]->get_weights();

            MPI_Status sta;
            int flag;

            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &sta);
            if (flag)
            {
                if (sta.MPI_TAG == TAG_KILL)
                {
                    printf("Process #%d was ordered to be killed by #%d.\n", config->rank, sta.MPI_SOURCE);
                    return true;
                }
            }

            if (w == hidden_weights)
            {
                printf("Offline attack succeeded. Weights found:\n");
                trees[a]->print_weights();
                sent_kill_order(config->rank);
                return true;
            }

        }
    }

    return false;
}

bool 
control_t::load_offline_data()
{
    ifstream file;
    file.open(trace_filename);

    if ( ! file.is_open())
    {
        printf("File %s is not valid or does not exist. Shutting down...\n", trace_filename.c_str());
        exit(1);
    }

    // here all data all loaded into single vector
    // data is divided into blocks
    // one block consists of:
    //  all input values
    //  output of tree1
    //  output of tree2
    //  intermediate values from tree1
    //  intermediate values from tree2
    int val;
    while (file >> val)
        offline_data.push_back(val);

    file.close();

    // weights data
    file.open(weights_filename);

    if ( ! file.is_open())
    {
        printf("File %s is not valid or does not exist. Shutting down...\n", weights_filename.c_str());
        exit(1);
    }

    while (file >> val)
        hidden_weights.push_back(val);

    file.close();

    return true;
}

void 
control_t::sent_kill_order(int p_my_rank)
{
    int v = 0;
    
    for (int p = 0; p < config->nodes; p++)
    {
        if (p_my_rank != p)
        {
            MPI_Send(&v, 1, MPI_INT, p, TAG_KILL, MPI_COMM_WORLD);
            printf("Sent kill order from %d to %d proc.\n", p_my_rank, p);
        }
    }
}

bool
control_t::run_serial()
{
    ofstream file, w_file;
    file.open (trace_filename);
    w_file.open (weights_filename);

    if (config->get_type() == run_type_t::ONLINE_ATTACK_SERIAL)
    {
        trees = new tree_t*[config->get_attackers()];

        for (int a = 0; a < config->get_attackers(); a++)
            trees[a] = new tree_t(config->get_perceptrons(), config->get_inputs(), config->get_range());        
    }

    for (int s = 0; s < config->get_iterations(); s++)
    {
        vector<int> input = random_vector(config->get_all_inputs());
        
        for (int o = 0; o < input.size(); o++)
            file << input[o] << "\t";

        if (s == 0)
        {
            tree1->evaluate(input, true, true);
            tree2->evaluate(input, true, true);

            if (config->get_type() == run_type_t::ONLINE_ATTACK_SERIAL)
                for (int a = 0; a < config->get_attackers(); a++)
                    trees[a]->evaluate(input, true, true);
        }

        srand(get_random_seed());

        int t1_eval = tree1->evaluate(input, true, false);
        int t2_eval = tree2->evaluate(input, true, false);

        file << t1_eval << "\t" << t2_eval << "\t";

        for (int l = 0; l < tree1->K; l++)
        {
            file << tree1->head->children[l]->value << "\t";
        }

        for (int l = 0; l < tree2->K; l++)
        {
            file << tree2->head->children[l]->value << "\t";
        }

        if (t1_eval == t2_eval)
        {
            tree1->update_weights();
            tree2->update_weights();

            if (config->get_type() == run_type_t::ONLINE_ATTACK_SERIAL)
            {
                for (int a = 0; a < config->get_attackers(); a++)
                {
                    if (tree1->evaluate(input, true, false) == trees[a]->evaluate(input, true, false))
                    {
                        trees[a]->update_weights();
                    }
                    else
                    {   
                        trees[a]->find_min();
                        trees[a]->update_weights();
                    }
                }
            }
        }

        if (config->get_type() == run_type_t::ONLINE_ATTACK_SERIAL)
        {
            if (check_attack_success())
            {
                printf("Attack was successful.\n");
                return true;
            }
        }

        if (check_protocol_synchronisation())
        {
            std::vector<int> result = tree1->get_weights();
            for (int i = 0; i < result.size(); i++)
                w_file << result[i] << "\t";

            printf("Trees 1 and 2 synchronised. Number of iterations needed: %d.\n", s);
            return true;
        }

        file << "\n";
    }

    w_file.close();
    file.close();
    return false;
}

double 
control_t::get_difference(double p_end, double p_start)
{
    return (p_end * 1000) - (p_start * 1000);
}

int 
control_t::get_block_size()
{
    return config->get_all_inputs() + 2 * config->get_perceptrons() + 2;
}

std::vector<int> 
control_t::get_block(std::vector<int> p_data, int p_id_start, int p_id_length)
{
    std::vector<int> block;

    for (int i = p_id_start; i < p_id_start + p_id_length; i++)
        block.push_back(p_data[i]);

    return block;
}

void 
control_t::fill_block(std::vector<int> p_line, block_t& p_block)
{
    std::vector<int> temp;
    p_block.inputs = get_block(p_line, 0, config->get_all_inputs());
    
    temp = get_block(p_line, config->get_all_inputs(), 1);
    p_block.tree1_eval = temp[0];
    
    temp = get_block(p_line, config->get_all_inputs() + 1, 1);
    p_block.tree2_eval = temp[0];

    p_block.tree1_intermediate = get_block(p_line, config->get_all_inputs() + 2, config->get_perceptrons());
    p_block.tree2_intermediate = get_block(p_line, config->get_all_inputs() + 2 + config->get_perceptrons(), config->get_perceptrons());

}