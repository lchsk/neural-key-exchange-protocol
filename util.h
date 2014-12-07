#ifndef UTIL_H
#define UTIL_H

// Generates n random values {-1, 1}
std::vector<int> random_vector(int n);

int get_random_seed();

// Adjusts p_value to make sure it's between min and max
int clamp(int p_value, int p_min, int p_max);

#endif