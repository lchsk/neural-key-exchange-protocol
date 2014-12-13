#include "node.h"

node_t::node_t(
    tree_t* p_tree, 
    node_t* p_parent, 
    node_type_t p_type, 
    int p_weight, 
    int p_value,
    int p_id
) : tree(p_tree), parent(p_parent), weight(p_weight), value(p_value), type(p_type), id(p_id)
{
    switch(type)
    {
        case node_type_t::OUTPUT:

            children = new node_t*[tree->K];

            for (int i = 0; i < tree->K; ++i)
            {
                children[i] = new node_t(tree, this, node_type_t::PERCEPTRON, 0, i, 0);
            }

        break;

        case node_type_t::PERCEPTRON:

            children = new node_t*[tree->n];

            for (int index = 0, i = value * tree->n; index < tree->n; i++, index++)
            {   
                children[index] = new node_t(tree, this, node_type_t::INPUT, 0, i, i);
            }

        break;
    }
}

node_t::~node_t()
{
    delete[] children;
}