/************************************************** INCLUDE **************************************************/

#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************************************** DEFINE **************************************************/
#define BUFFER_SIZE		200
#define ROOT 			0
#define BLOCK_LOW(rank,numprocs,size) 	((rank)*(size)/(numprocs))
#define BLOCK_HIGH(rank,numprocs,size) 	(BLOCK_LOW((rank)+1,numprocs,size)-1)
#define BLOCK_SIZE(rank,numprocs,size) 	(BLOCK_HIGH(rank,numprocs,size)-BLOCK_LOW(rank,numprocs,size)+1)

/************************************************** STRUCT NODE **************************************************/
struct node
{
	int a;
	int b;
	int c;
};

/************************************************** MAIN **************************************************/
int main(int argc, char *argv[])
{
	MPI_Aint 		struct_displs[3] = 	{offsetof(struct node, a), offsetof(struct node, b), offsetof(struct node, c)};
	MPI_Aint 		struct_extent;
	MPI_Aint 		struct_lb;
	MPI_Datatype 	MPI_CELL;
	MPI_Datatype 	MPI_CELL_t;
	MPI_Datatype 	struct_type[3] 	= 	{MPI_INT, MPI_INT, MPI_INT};
	struct node 	***global_cube 	= 	NULL;
	struct node 	***local_cube 	= 	NULL;
	struct node 	*global_cube_t 	= 	NULL;
	struct node 	*local_cube_t 	= 	NULL;
	int 			*displs 		= 	NULL;
	int 			*sendcounts 	= 	NULL;
	int 			struct_b_len[3] = 	{1, 1, 1};
	int 			iterations 		= 	0;
	int 			numprocs 		= 	0;
	int 			rank 			= 	0;
	int 			size 			= 	0;
	int 			i 				= 	0;
	int 			j 				= 	0;
	int 			x 				= 	0;
	int 			y 				= 	0;
	int 			z 				= 	0;


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Type_create_struct(3, struct_b_len, struct_displs, struct_type, &MPI_CELL_t);
	MPI_Type_get_extent(MPI_CELL_t, &struct_lb, &struct_extent);
	MPI_Type_create_resized(MPI_CELL_t, struct_lb, struct_extent, &MPI_CELL);
	MPI_Type_commit(&MPI_CELL);

	if(rank == ROOT)
	{
		FILE 	*input_fd 			= NULL;
		char 	*input_filename 	= NULL;
		char 	buffer[BUFFER_SIZE] = {0};
		/************************************************** PARSE_ARGS **************************************************/
		input_filename = (char *) malloc((strlen(argv[1]) * sizeof(char)) + 1);
		strcpy(input_filename, argv[1]);
		iterations = atoi(argv[2]);
		/************************************************** READ_SIZE **************************************************/
		input_fd = fopen(input_filename, "r");
		free(input_filename);
		fgets(buffer,BUFFER_SIZE,input_fd);
		sscanf(buffer,"%d", &size);
		/************************************************** BCAST_SIZE **************************************************/
		MPI_Bcast(&size, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
		/************************************************** INIT_STRUCTS **************************************************/
		global_cube 	= (struct node ***) malloc(size * sizeof(struct node **));
		global_cube_t 	= (struct node *) malloc(size * size * size * sizeof(struct node));
		memset(global_cube, 0, size * sizeof(struct node **));
		memset(global_cube_t, 0, size * size * size * sizeof(struct node));

		for(x = 0; x < size; x++)
		{
			global_cube[x] = (struct node **) malloc(size * sizeof(struct node *));
			for(y = 0; y < size; y++)
			{
				global_cube[x][y] = &global_cube_t[(size * size * x) + (size * y)];
				for(z = 0; z < size; z++)
				{
					global_cube[x][y][z].a = -1;
					global_cube[x][y][z].b = -1;
					global_cube[x][y][z].c = -1;
				}
			}
		}
		/************************************************** READ_INPUT **************************************************/
		while(fgets(buffer,BUFFER_SIZE,input_fd) != NULL)
		{
			sscanf(buffer,"%d %d %d", &x, &y, &z);
			global_cube[x][y][z].a = x;
			global_cube[x][y][z].b = y;
			global_cube[x][y][z].c = z;
		}
		fclose(input_fd);
		/************************************************** SCATTER_DATA **************************************************/
		displs 		= (int *) malloc(numprocs * sizeof(int));
		sendcounts 	= (int *) malloc(numprocs * sizeof(int));
		memset(displs, 0, numprocs * sizeof(int));
		memset(sendcounts, 0, numprocs * sizeof(int));
		for(i = 0; i < numprocs; i++)
		{
			sendcounts[i] = BLOCK_SIZE(i, numprocs, size) * size * size;
			displs[i+1] = displs[i] + sendcounts[i];
		}
		local_cube_t = (struct node *) malloc(BLOCK_SIZE(rank, numprocs, size) * size * size * sizeof(struct node));
		memset(local_cube_t, 0, BLOCK_SIZE(rank, numprocs, size) * size * size * sizeof(struct node));
		MPI_Scatterv(global_cube_t, sendcounts, displs, MPI_CELL, local_cube_t, BLOCK_SIZE(rank, numprocs, size) * size * size, MPI_CELL, ROOT, MPI_COMM_WORLD);
		for(i = 0; i < BLOCK_SIZE(rank, numprocs, size) * size * size; i++)
		{
			if(local_cube_t[i].a != -1)
			{
				fprintf(stdout,"Process %d - (%d %d %d)\n", rank, local_cube_t[i].a, local_cube_t[i].b, local_cube_t[i].c);
			}
		}
	}
	else
	{
		MPI_Bcast(&size, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

		local_cube_t = (struct node *) malloc(BLOCK_SIZE(rank, numprocs, size) * size * size * sizeof(struct node));
		memset(local_cube_t, 0, BLOCK_SIZE(rank, numprocs, size) * size * size * sizeof(struct node));
		MPI_Scatterv(global_cube_t, sendcounts, displs, MPI_CELL, local_cube_t, BLOCK_SIZE(rank, numprocs, size) * size * size, MPI_CELL, ROOT, MPI_COMM_WORLD);
		for(i = 0; i < BLOCK_SIZE(rank, numprocs, size) * size * size; i++)
		{
			if(local_cube_t[i].a != -1)
			{
				fprintf(stdout,"Process %d - (%d %d %d)\n", rank, local_cube_t[i].a, local_cube_t[i].b, local_cube_t[i].c);
			}
		}
	}
	fprintf(stdout,"Process - %d Size - %d\n", rank, BLOCK_SIZE(rank, numprocs, size));

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();



	return 0;

}
