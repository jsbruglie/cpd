#ifndef DIST_GRID_H
#define DIST_GRID_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include "graph.h"

#define ROOT 0 /**< Macro for representing the root process in MPI */
//#define VERBOSE 1

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

/** @brief Parse command line arguments
 *
 *  @param argc Number of arguments
 *  @param argv Argument string
 *  @param file Output
 */
void parseArgs(int argc, char* argv[], char** file, int* generations);

#endif
