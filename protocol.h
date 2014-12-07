#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>

enum class node_type_t {
    OUTPUT,
    PERCEPTRON,
    INPUT
};

class tree_t;

class node_t{

    public:
        node_t(
            tree_t* p_tree, 
            node_t* p_parent, 
            node_type_t p_type, 
            int p_weight, 
            int p_value,
            int p_id
        );
        ~node_t();
        node_t* parent;
        node_t** children;
        node_type_t type;
        
        tree_t* tree;

        // weight (from this node to its parent)
        int weight;

        int value;

        // unique for each input node
        int id;


    private:
        
};

class tree_t{

    public:
        tree_t(int p_K, int p_n, int p_L);
        ~tree_t();
        void evaluate(std::vector<int> v);

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

std::vector<int> random_vector(int p_elements);

#endif