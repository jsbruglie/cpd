/** @file dist_grids.c
*  @brief Distributed MPI version of bruteforce in C
*  @author Pedro Abreu
*  @author João Borrego
*  @author Miguel Cardoso
*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++ INCLUDES +++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

/*+++++++++++++++++++++++++++++++++++++++++++++++++ MACROS +++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define BUFFER_SIZE 200
#define ROOT 0
#define WORLD MPI_COMM_WORLD

/*+++++++++++++++++++++++++++++++++++++++++++++++++ BLOCK MACROS +++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define BLOCK_LOW(rank,numprocs,size) 	((rank)*(size)/(numprocs))
#define BLOCK_HIGH(rank,numprocs,size) 	(BLOCK_LOW((rank)+1,numprocs,size)-1)
#define BLOCK_SIZE(rank,numprocs,size) 	(BLOCK_HIGH(rank,numprocs,size)-BLOCK_LOW(rank,numprocs,size)+1)

/*+++++++++++++++++++++++++++++++++++++++++++++++++ STRUCTURES +++++++++++++++++++++++++++++++++++++++++++++++++++*/
typedef struct _node{
  int x;
	int y;
	int z;
}node;

typedef struct _list_node{
  node n;
  struct _list_node* next;
}list_node;

int main(int argc, char **argv) {

  int nprocs = 	0, rank = 0; //Number of processes and rank of the process
  node*** global_graph = NULL;
	node*** local_graph = NULL;
	node* global_cube_t = NULL;
	node* local_cube_t = NULL;



  MPI_Init(&argc, &argv);
    /*MPI PREAMBLE*/
    MPI_Comm_size(WORLD, &nprocs); //Assign rank and nprocess
    MPI_Comm_rank(WORLD, &rank);
    //MPI Create struct
    MPI_Type_create_struct(3, struct_b_len, struct_displs, struct_type, &MPI_CELL_t);
    MPI_Type_get_extent(MPI_CELL_t, &struct_lb, &struct_extent);
  	MPI_Type_create_resized(MPI_CELL_t, struct_lb, struct_extent, &MPI_CELL);
  	MPI_Type_commit(&MPI_CELL);

    if(rank==ROOT){
      /***************************************************** RAN BY ROOT *********************************************************/
      char* file;             /**< Input data file name */

    }else{
      /***************************************************** OTHER PROCS *********************************************************/



    }



    MPI_Barrier(MPI_COMM_WORLD);

  MPI_Finalize();
}
