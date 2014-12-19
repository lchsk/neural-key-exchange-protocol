#ifndef CONTROL_H
#define CONTROL_H

#include "config.h"
#include "protocol.h"

#include <vector>

class block_t
{

public:
    std::vector<int> inputs;
    int tree1_eval;
    int tree2_eval;
    std::vector<int> tree1_intermediate;
    std::vector<int> tree2_intermediate;


};

class control_t
{
    public:
        control_t(config_t* c);
        ~control_t();
        bool run();
        bool run_serial();
        bool run_parallel();
        bool check_protocol_synchronisation();
        bool check_attack_success();

        void sent_kill_order(int p_my_rank);

        // return number of values that are available
        // in a tree for an offline attack
        int get_block_size();

        // returns part of p_data of arbitrary length
        std::vector<int> get_block(std::vector<int> p_data, int p_id_start, int p_id_length);

        // file block with the data from line
        void fill_block(std::vector<int> p_line, block_t& p_block);

        bool load_offline_data();
        bool run_offline();

        double get_time(){ return MPI_Wtime(); }
        double get_difference(double p_end, double p_start);
        void print_time(double p_time);

    private:
        config_t* config;

        // network A
        tree_t* tree1;

        // network B
        tree_t* tree2;

        // attacking trees
        tree_t** trees;

        std::vector<int> offline_data;
        std::vector<int> hidden_weights;

        std::string trace_filename;
        std::string weights_filename;
};

#endif