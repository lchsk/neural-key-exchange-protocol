#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>

class node_t;

class tree_t{

    public:
        tree_t(int p_K, int p_n, int p_L);
        ~tree_t();
        void print_weights();
        int evaluate(std::vector<int> v, bool p_change_input, bool p_rand_weights);
        void update_weights();
        bool is_synchronised(tree_t& p_tree);

        // value used for input weights [-L, L]
        int L;

        // number of perceptrons
        int K;

        // number of input nodes in each perceptron
        int n;

        // number of input values (n * K)
        int N;

        node_t* head;
        std::vector<int> input;

    private:
};

#endif