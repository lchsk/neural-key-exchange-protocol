#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "main.h"
#include "config.h"
#include "definitions.h"
#include "protocol.h"
#include "util.h"
#include "control.h"

using namespace std;

int 
main (int argc, char* argv[])
{

    config_t c;

    c.read_parameters(argc, argv);

    control_t* control = new control_t(&c);
    control->run();


    delete control;

    return 0;
}