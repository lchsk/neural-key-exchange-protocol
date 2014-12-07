#include <cstdio>
#include <random>

#include "protocol.h"

tree_t::tree_t(int p_K, int p_n, int p_L) : K(p_K), n(p_n), N(K * n), L(p_L)
{
    //printf("params: %d, %d = %d\n", K, n, N);

    // create K perceptrons
    head = new node_t(this, NULL, node_type_t::OUTPUT, 0, 0, 0);
}

tree_t::~tree_t()
{
    delete head;
}

void 
tree_t::evaluate(std::vector<int> v)
{
    int output = 1;

    for (int i = 0; i < K; i++)
    {
        int value = 0;

        for (int j = 0; j < n; j++)
        {
            node_t* n = head->children[i]->children[j];
            n->value = v[n->id];

            n->weight = rand() % (L * 2 + 1) - L;

            printf("i: %d, id: %d, w: %d\n", n->value, n->id, n->weight);
        
            value += (n->value * n->weight);
        }

        printf("perceptron %d output is %d\n", i, value);

        if (value >= 0)
            head->children[i]->value = 1;
        else
            head->children[i]->value = -1;

        output *= head->children[i]->value;
    }

    head->value = output;
    printf("OUTPUT: %d\n", output);
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
            for (int index = 0, i = value * tree->n; index < tree->n; i++, index++)
            {   
                children[index] = new node_t(tree, this, node_type_t::INPUT, 0, i, i);
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
    {
        if (rand() % 2 == 0)
            v.push_back(1);
        else
            v.push_back(-1);
    }
        

    return v;
}