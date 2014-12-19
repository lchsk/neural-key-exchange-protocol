#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <mpi.h>

enum class run_type_t
{
    PROTOCOL_ONLY,
    ONLINE_ATTACK_SERIAL,
    ONLINE_ATTACK_MPI,
    OFFLINE_ATTACK
};

class config_t
{
    public:

        config_t();
        ~config_t();

        void read_parameters(int argc, char* argv[]);
        void print_parameters();
        void help();
        std::string get_program_type();

        // accessors
        int get_iterations(){ return iterations; }
        int get_attackers(){ return attackers; }
        int get_perceptrons(){ return K; }
        int get_inputs(){ return n; }
        int get_range(){ return L; }
        int get_all_inputs(){ return N; }
        run_type_t get_type(){ return type; }

        int rank;
        int nodes;

    private:
        void update();

        // number of iterations in geometric attack
        int iterations;

        // number of attackers
        int attackers;
        
        // number of intermediate nodes
        int K;

        // number of input values in each intermediate nodes
        int n;

        // range of weights
        int L;

        // number of all inputs (K * n)
        int N;

        // how the program should behave
        run_type_t type;

};

#endif