#ifndef CONFIG_H
#define CONFIG_H

class config_t
{
    public:

        config_t();
        ~config_t();

        void read_parameters(int argc, char* argv[]);
        void print_parameters();
        void help();

        // accessors
        int get_iterations(){ return iterations; }
        int get_perceptrons(){ return K; }
        int get_inputs(){ return n; }
        int get_range(){ return L; }
        int get_all_inputs(){ return N; }

    private:
        void update();

        // number of iterations in geometric attack
        int iterations;
        
        // number of intermediate nodes
        int K;

        // number of input values in each intermediate nodes
        int n;

        // range of weights
        int L;

        // number of all inputs (K * n)
        int N;


};

#endif