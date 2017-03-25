#ifndef SEQ_GRID_H
#define SEQ_GRID_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define ALIVE 1
#define DEAD 0

typedef char bool;
bool*** initGraph(int size);

void printActive(bool*** graph, int cube_size);

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

/* @ brief
 *
 */
bool*** parse_file(char* file, int* cube_size);    

#endif