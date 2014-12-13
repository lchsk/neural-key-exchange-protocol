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

            // weighted sum
            value += (n->value * n->weight);
        }

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
tree_t::update_subtree_weights(int p_subtree_id)
{
    // perceptron's inputs
    for (int j = 0; j < n; j++)
    {
        node_t* n = head->children[p_subtree_id]->children[j];
        n->weight = n->weight - head->children[p_subtree_id]->value * n->value;
        n->weight = clamp(n->weight, -L, L);
    }
}

void 
tree_t::update_weights()
{
    // all perceptrons
    for (int i = 0; i < K; i++)
    {
        if (head->value == head->children[i]->value)
        {
            update_subtree_weights(i);
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

void
tree_t::find_min()
{
    int min_value = 100 * K;
    int min_id = -1;
    int tmp;

    for (int i = 0; i < K; i++)
    {
        tmp = 0;

        for (int j = 0; j < n; j++)
        {
            node_t* node = head->children[i]->children[j];
            tmp += node->weight * node->value;
        }

        tmp = abs(tmp);
        if (tmp < min_value)
        {
            min_value = tmp;
            min_id = i;
        }
    }

    node_t* node = head->children[min_id];
    node->value = -node->value;
}