#ifndef SEQUENTIAL_GRID_H
#define SEQUENTIAL_GRID_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>


/*+++++++++++++++++++++++++++++++++++++++++++++++++ MACROS +++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define BUFFER_SIZE 200
#define INF 10000000
#define REMOVAL_PERIOD 5
#define ROOT 0
#define ALIVE 1
#define DEAD 0
#define true 1
#define false 0
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

void countFrontierSize(int rank, int nprocs, int size, graph_node*** local_graph, int* low_frontier_count, int* high_frontier_count);

void createFrontiers(int rank, int nprocs, int size, graph_node* it, graph_node*** local_graph, node** sending_low_frontier, node** sending_high_frontier);

void computeAdjacentRanks(int rank, int nprocs, int* low_rank, int* high_rank);

void printAndSortActive(graph_node*** graph, int size);

void graphNodeSort(graph_node** first_ptr);

void freeGraph(graph_node*** graph, int size);

void graphNodeDelete(graph_node* first);

#endif
