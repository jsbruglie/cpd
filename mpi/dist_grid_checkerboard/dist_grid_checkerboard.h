/**
 * @file dist_grid_checkerboard.c
 * @brief Game Of Life 3D MPI Implementation
 * 
 * @details Uses checkerboard decomposition to scatter the 
 * 3D graph across several processes.
 * If an invalid configuration is provided the program will not attempt
 * to run, and will notify the user instead.
 * 
 * @attention Each process is assumed to have access to the input file!
 *  
 * @author João Borrego
 * @author Pedro Abreu
 * @author Miguel Cardoso
 * @bug No known bugs.  
 */

#ifndef DIST_GRID_CHECKERBOARD_H
#define DIST_GRID_CHECKERBOARD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>

#include "graph.h"
#include "debug.h"

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

/**< Macro for identifying communication between current process and lower x neighbour */
#define TAG_LOW_X 100
/**< Macro for identifying communication between current process and higher x neighbour */
#define TAG_HIGH_X 200
/**< Macro for identifying communication between current process and lower y neighbour */
#define TAG_LOW_Y 300
/**< Macro for identifying communication between current process and higher y neighbour */
#define TAG_HIGH_Y 400

/* General Macros */

/**< Dead node removal period */
#define REMOVAL_PERIOD 5 

/**< Buffer Size */
#define BUFFER_SIZE 200 

/* Function headers */

/**
 * @brief Parse command line arguments
 *
 * @param argc Argument count
 * @param argv Argument values
 * @param file The input file name
 * @param generations The number of generations
 */
void parseArgs(int argc, char *argv[], char **file, int *generations);

/**
 * @brief Inserts a node in the local graph
 *
 * @param graph The local graph representation
 * @param offset_x The offset in coordinate x
 * @param offset_y The offset in coordinate y
 * @param x x global coordinate
 * @param y y global coordinate
 * @param z z global coordinate
 */
void insertLocalGraph(GraphNode ***graph, int offset_x, int offset_y, int x, int y, int z);

/**
 * @brief Adds a reduced node to the boundary array to be sent
 * 
 * @param array The array of boundary reduced nodes
 * @param index The index to insert reduced object in
 * @param a x or y local coordinate
 * @param z z global coordinate
 */
void addToSndArray(RNode *array, int index, int a, int z);

/**
 * @brief Sends a border to a neighbour process
 *
 * @param mpi_comm The MPI Communicator object
 * @param mpi_tag The monitored MPI tag
 * @param my_rank Origin process rank
 * @param nbr_rank Destination process rank
 * @param status_snd MPI Status object for sending
 * @param mpi_datatype MPI Datatype of object to be sent
 * @param snd The array to be sent
 * @param snd_size The size of the array to be sent
 */
void sendBorder(MPI_Comm mpi_comm, int mpi_tag, int my_rank, int nbr_rank,
            	MPI_Request *req, MPI_Datatype mpi_datatype,
                RNode *snd, int snd_size);

/**
 * @brief Sends a border from a neighbour process
 *
 * @param mpi_comm The MPI Communicator object
 * @param mpi_tag The monitored MPI tag
 * @param my_rank Destination process rank
 * @param nbr_rank Origin process rank
 * @param status_prb MPI Status object for probing to obtain size
 * @param status_rcv MPI Status object for receiving
 * @param mpi_datatype  MPI Datatype of object to be received
 * @param rcv The array to be received
 * @return The size of the array to be received
 */
int receiveBorder(MPI_Comm mpi_comm, int mpi_tag, int my_rank, int nbr_rank,
                    MPI_Request *req, MPI_Status *status_prb,
                    MPI_Datatype mpi_datatype, RNode **rcv);

#endif
