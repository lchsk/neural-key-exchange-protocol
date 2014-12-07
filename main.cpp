#include <iostream>
#include <cstdio>
#include "main.h"
#include "definitions.h"
#include "protocol.h"

using namespace std;

int 
main (int argc, char* argv[])
{
    vector<int> input = random_vector(15);

    // for (auto i : input)
    // {
    //     printf("val: %d\n", i);
    // }

    tree_t tree(3, 5, 3);
    tree.evaluate(input);


    return 0;
}