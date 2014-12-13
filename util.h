#ifndef UTIL_H
#define UTIL_H

#include "node.h"

// Generates n random values {-1, 1}
std::vector<int> random_vector(int n);

int get_random_seed();

// Adjusts p_value to make sure it's between min and max
int clamp(int p_value, int p_min, int p_max);

// Evaluates perceptron and returns true if it gives a positive (>= 0) value
bool is_positive(node_t* node, int n);

#endif