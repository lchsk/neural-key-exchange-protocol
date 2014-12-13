#include <ctime>
#include <vector>
#include <random>

#include "node.h"
#include "util.h"

std::vector<int>
random_vector(int p_elements)
{
    srand(get_random_seed());

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

int 
get_random_seed()
{
    // time_t now;
    // struct tm newyear;

    // time(&now);

    // newyear = *localtime(&now);

    // newyear.tm_hour = 0; newyear.tm_min = 0; newyear.tm_sec = 0;
    // newyear.tm_mon = 0;  newyear.tm_mday = 1;

    // return (int)difftime(now, mktime(&newyear));
    return clock();
}

int 
clamp(int p_value, int p_min, int p_max)
{
    if (p_value > p_max)
        return p_max;

    if (p_value < p_min)
        return p_min;

    return p_value;
}

bool 
is_positive(node_t* node, int n)
{
    int value = 0;

    for (int j = 0; j < n; j++)
    {
        value += (node->children[j]->value * node->children[j]->weight);
    }

    return value >= 0;
}