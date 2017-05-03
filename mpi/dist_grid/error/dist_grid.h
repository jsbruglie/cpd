#ifndef SEQUENTIAL_GRID_H
#define SEQUENTIAL_GRID_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define VERBOSE 1

#ifdef VERBOSE
#define debug_print(M, ...) do {printf("DEBUG: %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); fflush(stdout);} while(0)
#endif

#ifdef VERBOSE
#define rank_print(a) do{ printf("@RANK %d - ", a); fflush(stdout);}while(0)
#endif
/*+++++++++++++++++++++++++++++++++++++++++++++++++ MACROS +++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define BUFFER_SIZE 200
#define INF 10000000
#define REMOVAL_PERIOD 5
#define ROOT 0
#define ALIVE 1
#define DEAD 0
#define true 1
#define false 0

/*+++++++++++++++++++++++++++++++++++++++++++++++++ BLOCK MACROS +++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define BLOCK_LOW(rank,numprocs,size) 	((rank)*(size)/(numprocs))
#define BLOCK_HIGH(rank,numprocs,size) 	(BLOCK_LOW((rank)+1,numprocs,size)-1)
#define BLOCK_SIZE(rank,numprocs,size) 	(BLOCK_HIGH(rank,numprocs,size)-BLOCK_LOW(rank,numprocs,size)+1)
#define BLOCK_OWNER(element,numprocs,size) ((numprocs*(element+1)-1)/size)

/*+++++++++++++++++++++++++++++++++++++++++++++++++ STRUCTURES +++++++++++++++++++++++++++++++++++++++++++++++++++*/
typedef unsigned char bool;

typedef struct _node{
    int z;
    int y;
    int x;
}node;

/** @brief Structure for storing a node of the graph */
typedef struct _graph_node{
    int z;                   /**< z coordinate, x and y are implicitly mapped */
    int state;                     /**< State of a node cell (DEAD or ALIVE) */
    unsigned char neighbours;       /**< Neighbour counter */
    struct _graph_node* next; /**< Pointer to the next entry in the list */
}graph_node;

void parseArgs(int argc, char* argv[], char** file, int* generations);

graph_node* graphNodeInsert(graph_node* first, int z, int state);

graph_node*** initGraph(int size);

graph_node*** initLocalGraph(int bsize, int size);

bool graphNodeAddNeighbour(graph_node** first, int z);

void visitNeighbours(graph_node*** graph, int cube_size, int x, int y, int z);

void graphListCleanup(graph_node** head);


#endif
