#ifndef NODE_H
#define NODE_H

#include "protocol.h"

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
};

#endif