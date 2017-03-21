#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define X 0
#define Y 1
#define Z 2

typedef struct _cell{
	int state;
	int live_neighbours;
}cell;

void check_dimension(int dim, int i, int j, int k, int cube_size);