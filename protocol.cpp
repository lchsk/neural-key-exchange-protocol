#include <cstdio>
#include <random>

#include "protocol.h"

tree_t::tree_t(int p_K, int p_n) : K(p_K), n(p_n), N(K * n)
{
    printf("params: %d, %d = %d\n", K, n, N);

    // create K perceptrons
    head = new node_t(this, NULL, node_type_t::OUTPUT, 0, 0, 0);
}

tree_t::~tree_t()
{
    //delete[] head->children;
    delete head;
}

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

            //printf("children: %d\n", tree->K);
            children = new node_t*[tree->K];

            for (int i = 0; i < tree->K; ++i)
            {
                //printf("%i=== %p\n", i, children[i]);
                children[i] = new node_t(tree, this, node_type_t::PERCEPTRON, 0, i, 0);
                
            }

        break;

        case node_type_t::PERCEPTRON:

            children = new node_t*[tree->n];
            //printf("children: %d\n", tree->n);
            for (int i = value * tree->n; i < value * tree->n + tree->n; ++i)
            {   
                id = i;
                children[i] = new node_t(tree, this, node_type_t::INPUT, 0, i, i);
                //printf("%d, id: %d, %p\n", i, id, children[i]);
            }

        break;
    }
}

node_t::~node_t()
{
    delete[] children;
}

std::vector<int>
random_vector(int p_elements)
{
    srand(time(NULL));

    std::vector<int> v;

    for (int i = 0; i < p_elements; i++)
        v.push_back(rand() % 2);

    return v;
}