#include <iostream>
#include <cstdio>
#include "main.h"
#include "definitions.h"
#include "protocol.h"
#include "util.h"

using namespace std;

int 
main (int argc, char* argv[])
{
    tree_t tree1(3, 5, 3);
    tree_t tree2(3, 5, 3);

    int s = 0;
    while (true)
    {
        vector<int> input = random_vector(15);

        if (s == 0)
        {
            tree1.evaluate(input, true, true);
            tree2.evaluate(input, true, true);
        }

        srand(get_random_seed());

        if (tree1.evaluate(input, true, false) == tree2.evaluate(input, true, false))
        {
            tree1.update_weights();
            tree2.update_weights();
        }

        tree1.print_weights();
        tree2.print_weights();

        if (tree1.is_synchronised(tree2))
        {
            printf("Took %d iterations.\n", s);
            break;
        }
        s++;
    }

    return 0;
}