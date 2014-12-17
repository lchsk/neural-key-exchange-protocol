#include "control.h"
#include "config.h"
#include "definitions.h"
#include "protocol.h"
#include "util.h"
#include <vector>

using namespace std;

control_t::control_t(config_t* c) : config(c)
{
    tree1 = new tree_t(config->get_perceptrons(), config->get_inputs(), config->get_range());
    tree2 = new tree_t(config->get_perceptrons(), config->get_inputs(), config->get_range());
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

        // int flag;
        // MPI_Test(&req, &flag, &sta);
        // if (sta.MPI_TAG == TAG_KILL)
        // printf("COME ON BRO %d\n", flag);
        //MPI_Wait(&req, &sta);
        //if (status.MPI_TAG == TAG_KILL) {
         //   printf("Process %d exiting work loop.\n", config->rank);
         //   return true;
        //}

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
    if (config->get_type() == run_type_t::ONLINE_ATTACK_SERIAL)
    {
        trees = new tree_t*[config->get_attackers()];

        for (int a = 0; a < config->get_attackers(); a++)
            trees[a] = new tree_t(config->get_perceptrons(), config->get_inputs(), config->get_range());        
    }

    for (int s = 0; s < config->get_iterations(); s++)
    {
        vector<int> input = random_vector(config->get_all_inputs());

        if (s == 0)
        {
            tree1->evaluate(input, true, true);
            tree2->evaluate(input, true, true);

            if (config->get_type() == run_type_t::ONLINE_ATTACK_SERIAL)
                for (int a = 0; a < config->get_attackers(); a++)
                    trees[a]->evaluate(input, true, true);
        }

        srand(get_random_seed());

        if (tree1->evaluate(input, true, false) == tree2->evaluate(input, true, false))
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
            printf("Trees 1 and 2 synchronised.\n");
            return true;
        }

    }

    return false;
}

double 
control_t::get_difference(double p_end, double p_start)
{
    return (p_end * 1000) - (p_start * 1000);
}