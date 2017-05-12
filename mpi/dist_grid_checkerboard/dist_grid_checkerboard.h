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

/* Communication tags */

/**< Macro for representing border object communications */
#define TAG_BORDER 10
/**< Macro for identifying communication between current process and lower x neighbour */
#define TAG_LOW_X 20
/**< Macro for identifying communication between current process and higher x neighbour */
#define TAG_HIGH_X 30
/**< Macro for identifying communication between current process and lower y neighbour */
#define TAG_LOW_Y 40
/**< Macro for identifying communication between current process and higher y neighbour */
#define TAG_HIGH_Y 50

/**< Dead node removal period */
#define REMOVAL_PERIOD 5 

/* TODO - Migrate to debug.h */

#define VERBOSE 1

#ifdef VERBOSE
/** */
#define debug_print(M, ...) do {printf("DEBUG: %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); fflush(stdout);} while(0)
#else
#define debug_print(M, ...)
#endif

#ifdef VERBOSE
/** */
#define rank_print(a) do{ printf("@RANK %d - ", a); fflush(stdout);}while(0)
#else
#define rank_print(a)
#endif

/* Function headers */

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param argc [description]
 * @param argv [description]
 * @param file [description]
 * @param generations [description]
 */
void parseArgs(int argc, char* argv[], char** file, int* generations);

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param graph [description]
 * @param offset_x [description]
 * @param offset_y [description]
 * @param x [description]
 * @param y [description]
 * @param z [description]
 */
void insertLocalGraph(GraphNode ***graph, int offset_x, int offset_y, int x, int y, int z);

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param array [description]
 * @param index [description]
 * @param x [description]
 * @param y [description]
 * @param z [description]
 */
void addToSndArray(Node *array, int index, int x, int y, int z);

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param mpi_comm [description]
 * @param mpi_tag [description]
 * @param my_rank [description]
 * @param nbr_rank [description]
 * @param status_snd [description]
 * @param mpi_datatype [description]
 * @param snd [description]
 * @param snd_size [description]
 */
void sendBorder(MPI_Comm mpi_comm, int mpi_tag, int my_rank, int nbr_rank,
                MPI_Status *status_snd, MPI_Datatype mpi_datatype,
                Node *snd, int snd_size);

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param mpi_comm [description]
 * @param mpi_tag [description]
 * @param my_rank [description]
 * @param nbr_rank [description]
 * @param status_prb [description]
 * @param status_rcv [description]
 * @param mpi_datatype [description]
 * @param rcv [description]
 * @return [description]
 */
int receiveBorder(MPI_Comm mpi_comm, int mpi_tag, int my_rank, int nbr_rank,
                    MPI_Status *status_prb, MPI_Status *status_rcv,
                    MPI_Datatype mpi_datatype, Node **rcv);

#endif