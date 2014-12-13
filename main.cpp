#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "main.h"
#include "config.h"
#include "definitions.h"
#include "protocol.h"
#include "util.h"

using namespace std;

int 
main (int argc, char* argv[])
{
    config_t c;
    c.read_parameters(argc, argv);
    c.print_parameters();

    return 0;

    tree_t tree1(c.get_perceptrons(), c.get_inputs(), c.get_range());
    tree_t tree2(c.get_perceptrons(), c.get_inputs(), c.get_range());
    tree_t** trees = new tree_t*[c.get_iterations()];

    for (int a = 0; a < c.get_iterations(); a++)
        trees[a] = new tree_t(c.get_perceptrons(), c.get_inputs(), c.get_range());

    int s = 0;
    while (true)
    {
        vector<int> input = random_vector(c.get_all_inputs());

        if (s == 0)
        {
            tree1.evaluate(input, true, true);
            tree2.evaluate(input, true, true);
            for (int a = 0; a < c.get_iterations(); a++)
                trees[a]->evaluate(input, true, true);
        }

        srand(get_random_seed());

        if (tree1.evaluate(input, true, false) == tree2.evaluate(input, true, false))
        {
            tree1.update_weights();
            tree2.update_weights();

            for (int a = 0; a < c.get_iterations(); a++)
            {
                if (tree1.evaluate(input, true, false) == trees[a]->evaluate(input, true, false))
                {
                    trees[a]->update_weights();
                }
                else
                {   
                    trees[a]->find_min();
                    trees[a]->update_weights();
                }
            }
            

        }

        for (int a = 0; a < c.get_iterations(); a++)
        {
            if (tree1.is_synchronised(*trees[a]))
            {
                printf("Tree %d synchronised\n\n\n\n", a);
                return 0;
            }
        }

        if (tree1.is_synchronised(tree2))
        {
            printf("Took %d iterations.\n", s);
            tree1.print_weights();
            tree2.print_weights();

            break;
        }
        s++;
    }

    delete[] trees;
    return 0;
}