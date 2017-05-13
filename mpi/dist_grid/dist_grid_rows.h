#ifndef DIST_GRID_ROWS_H
#define DIST_GRID_ROWS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <stdint.h>
#include <math.h>
#include "graph.h"
#include "debug.h"

#define ROOT 0 /**< Macro for representing the root process in MPI */

/** @brief Structure for sending over MPI */
typedef struct _neighbour_node{
    uint16_t z;
    uint16_t y;
}neighbour_node;

/** @brief Structure for sending over MPI */
typedef struct _node{
    uint16_t z;
    uint16_t y;
    uint16_t x;
}node;

/** @brief Parse command line arguments
 *
 *  @param argc Number of arguments
 *  @param argv Argument string
 *  @param file Output
 */
void parseArgs(int argc, char* argv[], char** file, int* generations);

#endif
