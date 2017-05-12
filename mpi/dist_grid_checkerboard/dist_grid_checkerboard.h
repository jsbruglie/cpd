#ifndef DIST_GRID_CHECKERBOARD_H
#define DIST_GRID_CHECKERBOARD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include "graph.h"

/* MPI Cartesian Mapping Definitions */

/**< MPI Cartesian Grid Dimensionality */
#define N_DIMS 2
/**< MPI_Cart_Shift along rows */
#define SHIFT_ROW 0
/**< MPI_Cart_Shift along columns */
#define SHIFT_COL 1
/**< Displacement - Upwards shift */
#define DISP 1

/* MPI Generic definitions */

/**< Macro for representing the root process in MPI */
#define ROOT 0


/* TODO - Migrate to debug.h */

#define VERBOSE 1

#ifdef VERBOSE
#define debug_print(M, ...) do {printf("DEBUG: %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); fflush(stdout);} while(0)
#else
#define debug_print(M, ...)
#endif

#ifdef VERBOSE
#define rank_print(a) do{ printf("@RANK %d - ", a); fflush(stdout);}while(0)
#else
#define rank_print(a)
#endif

/* Function headers */

/** @brief Parse command line arguments
 *
 *  @param argc Number of arguments
 *  @param argv Argument string
 *  @param file Output
 */
void parseArgs(int argc, char* argv[], char** file, int* generations);

#endif
