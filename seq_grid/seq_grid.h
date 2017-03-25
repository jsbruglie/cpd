#ifndef SEQ_GRID_H
#define SEQ_GRID_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "list.h"


void section1(Node**** graph0, Node**** graph1, int cube_size);

void section2(Node**** graph0, Node**** graph1, int cube_size);

void parse_args(int argc, char* argv[], char** file, int* generations);

#endif