#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "debug.h"

#define ALIVE 1
#define DEAD 0

typedef char bool;

/* @brief Initializes the graph representation structure
 *
 */
bool*** initGraph(int size);

/* @brief
 *
 */
void printActive(bool*** graph, int cube_size);

/* @brief
 *
 */
void freeGraph(bool*** graph, int size);

/* @brief
 *
 */
bool setNextState(bool*** graph, int cube_size, int x, int y, int z);

/* @brief
 *
 */
int liveNeighbors(bool*** graph, int cube_size, int x, int y, int z);

/* @brief
 *
 */
void parse_args(int argc, char* argv[], char** file, int* generations);

/* @brief
 *
 */
bool*** parse_file(char* file, int* cube_size);    

#endif