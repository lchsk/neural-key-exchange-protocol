#include <cstdio>
#include <random>


#include "protocol.h"
#include "node.h"
#include "util.h"

tree_t::tree_t(int p_K, int p_n, int p_L) : K(p_K), n(p_n), N(K * n), L(p_L)
{
    // create K perceptrons
    head = new node_t(this, NULL, node_type_t::OUTPUT, 0, 0, 0);
}

tree_t::~tree_t()
{
    delete head;
}

int
tree_t::evaluate(std::vector<int> v, bool p_change_input, bool p_rand_weights)
{
    srand(get_random_seed());
    int output = 1;

    for (int i = 0; i < K; i++)
    {
        int value = 0;

        for (int j = 0; j < n; j++)
        {
            node_t* n = head->children[i]->children[j];
            
            if (p_change_input)
                n->value = v[n->id];

            if (p_rand_weights)
                n->weight = rand() % (L * 2 + 1) - L;
            

            //printf("i: %d, id: %d, w: %d\n", n->value, n->id, n->weight);
            
            // weighted sum
            value += (n->value * n->weight);
        }

        //printf("perceptron %d output is %d\n", i, value);

        if (value >= 0)
            head->children[i]->value = 1;
        else
            head->children[i]->value = -1;

        output *= head->children[i]->value;
    }

    head->value = output;
    return head->value;
}

void 
tree_t::update_weights()
{
    // all perceptrons
    for (int i = 0; i < K; i++)
    {
        if (head->value  == head->children[i]->value)
        {
            // perceptron's inputs
            for (int j = 0; j < n; j++)
            {
                node_t* n = head->children[i]->children[j];
                n->weight = n->weight - head->children[i]->value *  n->value;
                n->weight = clamp(n->weight, -L, L);
            }
        }
    }
}

void
tree_t::print_weights()
{
    for (int i = 0; i < K; i++)
    {
        for (int j = 0; j < n; j++)
        {
            node_t* n = head->children[i]->children[j];
            printf("%d ", n->weight);
        }

        printf("\t");
    }
    printf("\n");
}

bool 
tree_t::is_synchronised(tree_t& p_tree)
{
    for (int i = 0; i < K; i++)
    {
        for (int j = 0; j < n; j++)
        {
            node_t* n1 = head->children[i]->children[j];
            node_t* n2 = p_tree.head->children[i]->children[j];
            
            if (n1->weight != n2->weight)
                return false;
        }
    }

    return true;
}

