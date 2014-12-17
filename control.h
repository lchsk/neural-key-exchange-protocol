#ifndef CONTROL_H
#define CONTROL_H

#include "config.h"
#include "protocol.h"

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
};

#endif