#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "lists.h"

#define ALIVE 1
#define DEAD 0

typedef unsigned char bool;

void visitNeighbours(GraphNode*** graph, int cube_size, List* list, coordinate x, coordinate y, coordinate z);
void insertNeighbours(GraphNode*** graph, int cube_size, coordinate x, coordinate y, coordinate z);


GraphNode*** initGraph(int size);



void printActive(GraphNode*** graph, int cube_size);

void freeGraph(GraphNode*** graph, int size);

/* @brief
 *
 */
void parse_args(int argc, char* argv[], char** file, int* generations);

/* @ brief
 *
 */
GraphNode*** parse_file(char* file, List* list, int* cube_size);    

#endif