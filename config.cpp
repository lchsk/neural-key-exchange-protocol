#include "config.h"
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <string>

config_t::config_t()
{
    // default values
    K = 3;
    L = 2;
    n = 3;
    N = n * K;
    iterations = 10000;
    attackers = 5;
    type = run_type_t::PROTOCOL_ONLY;
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

    printf("\nType: %s\n", get_program_type().c_str());

    printf("\nNumber of input values in each perceptron: %d\n", n);
    printf("Number of perceptrons: %d\n", K);
    printf("Number of iterations: %d\n", iterations);
    printf("Number of attackers: %d\n", attackers);
    printf("Range of weights: %d\n", L);
    printf("\n");
}

std::string config_t::get_program_type()
{
    if (type == run_type_t::PROTOCOL_ONLY)
        return "KKK protocol only";
    else if (type == run_type_t::ONLINE_ATTACK_SERIAL)
        return "Online attack (serial)";
    else if (type == run_type_t::ONLINE_ATTACK_MPI)
        return "Online attack (MPI)";
    else if (type == run_type_t::OFFLINE_ATTACK)
        return "Offline attack";

    return "";
}

void config_t::help()
{
    printf("Welcome to help screen. Below, each parameter is explained.\n");
    printf("-t [protocol/online-serial/online-mpi/offline] program's mode\n");
    printf("-K integer number of perceptrons in a tree\n");
    printf("-m integer number of input values\n");
    printf("-L integer range of weights\n");
    printf("-i integer number of iterations\n");
    printf("-a integer number of attackers\n");

    exit(0);
}

void config_t::read_parameters(int argc, char* argv[])
{
    int c;
    
    optind = 1;
    while ((c = getopt (argc, argv, "K:m:L:i:ht:a:")) != -1)
        switch (c)
        {
            case 'K':
                K = atoi(optarg);
                break;
            
            case 'm':
                n = atoi(optarg);
                break;
            
            case 'L':
                L = atoi(optarg);
                break;

            case 'i':
                iterations = atoi(optarg);
                break;

            case 'a':
                attackers = atoi(optarg);
                break;

            case 'h':
                help();
                break;

            case 't':
                {
                    std::string s = optarg;

                    if (s == "protocol")
                        type = run_type_t::PROTOCOL_ONLY;
                    else if (s == "online-serial")
                        type = run_type_t::ONLINE_ATTACK_SERIAL;
                    else if (s == "online-mpi")
                        type = run_type_t::ONLINE_ATTACK_MPI;
                    else if (s == "offline")
                        type = run_type_t::OFFLINE_ATTACK;

                    break;
                }

            // case '?':
                // break;
            default:
                help();
      }
}