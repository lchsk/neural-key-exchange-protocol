#include "config.h"
#include <unistd.h>
#include <cstdlib>
#include <cstdio>

config_t::config_t()
{
    // default values
    K = 3;
    L = 3;
    n = 3;
    N = n * K;
    iterations = 1;
}

config_t::~config_t()
{}

void config_t::update()
{
    N = n * K;
}

void config_t::print_parameters()
{
    printf("Program is running with the following parameters:\n");
    printf("(to see the description of each parameter run the program with -h argument)\n");

    printf("\nNumber of input values in each perceptron: %d\n", n);
    printf("Number of perceptrons: %d\n", K);
    printf("Number of iterations: %d\n", iterations);
    printf("Range of weights: %d\n", L);
}

void config_t::help()
{
    printf("HELP!\n");
    exit(0);
}

void config_t::read_parameters(int argc, char* argv[])
{
    int c;

    while ((c = getopt (argc, argv, "K:n:L:i:h")) != -1)
        switch (c)
        {
            case 'K':
                K = atoi(optarg);
                break;
            
            case 'n':
                n = atoi(optarg);
                break;
            
            case 'L':
                L = atoi(optarg);
                break;

            case 'i':
                iterations = atoi(optarg);
                break;

            case 'h':
                help();
                break;

            case '?':
                // if (optopt == 'c')
                //     fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        //         if (isprint (optopt))
        //   fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        // else
        //   fprintf (stderr,
        //            "Unknown option character `\\x%x'.\n",
        //            optopt);
        // return 1;
            default:
                help();
      }
}