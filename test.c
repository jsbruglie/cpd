#include "test.h"

cell ***M;

// To run ./test simple.in 2	

int main(int argc, char* argv[]){
	char* fileName = argv[1];
	int number_generations = atoi(argv[2]);
	FILE* fp = fopen(fileName, "r");

	int first = 0;
	int cube_size;
	char line[7]; //Only need 7 fo x y z
	int x, y, z; //Coordinates
	int i, j, k; //Iterators

	while(fgets(line, sizeof(line), fp)){ //fgets doesn't strip \n
		if(!first){
			if(sscanf(line, "%d\n", &cube_size) == 1){
				first = 1;
				M=(cell***)malloc(cube_size*sizeof(cell**));
				for(i=0;i<cube_size;i++){
					M[i] = (cell**)malloc(cube_size * sizeof(cell*));
					for(j=0;j<cube_size;j++){
						M[i][j] = (cell*)malloc(cube_size * sizeof(cell));
						for(k=0;k<cube_size;k++){ //Z
							M[i][j][k].state = 0;
							M[i][j][k].live_neighbours = 0;
						}
					}
				}	
			}

		}else{
			if(sscanf(line, "%d %d %d\n", &x, &y, &z) == 3)
				M[x][y][z].state = 1;
		}
	}
	int g;

	for(g=0; g<number_generations; g++){
		//printf("Generation #: %d\n",g);
		for(i=0;i<cube_size;i++){ //X
			for(j=0;j<cube_size;j++){ //Y
				for(k=0;k<cube_size;k++){ //Z
					check_dimension(X,i,j,k,cube_size);
					check_dimension(Y,i,j,k,cube_size);
					check_dimension(Z,i,j,k,cube_size);
				}
			}
		}
		//We need to wait for all these computations to be done, cannot evaluate while computing this
		for(i=0;i<cube_size;i++){
			for(j=0;j<cube_size;j++){
				for(k=0;k<cube_size;k++){
					if(M[i][j][k].state == 1){ //live cell
						if(M[i][j][k].live_neighbours < 2 || M[i][j][k].live_neighbours > 4){ //A live cell with fewer than two live neighbors dies and A live cell with more than four live neighbors dies
							M[i][j][k].state = 0; //kills cell
							//printf("Killing cell %d,%d,%d\n", i,j,k);
						}//A live cell with two to four live neighbours lives on to the next generation (simply remains 1, no need to check)
					}else{
						//A dead cell with two or three live neighbors becomes a live cell
						if(M[i][j][k].live_neighbours == 2 || M[i][j][k].live_neighbours == 3){
							M[i][j][k].state = 1; //becomes live cell
							//printf("Reviving cell %d,%d,%d\n", i,j,k);
						}
					}
					M[i][j][k].live_neighbours = 0; //After checking reset live_neighbours for another gen
				}
			}
		}
	}
	//Print the cells that are alive
	for(i=0;i<cube_size;i++) //X
			for(j=0;j<cube_size;j++) //Y
				for(k=0;k<cube_size;k++) //Z
					if(M[i][j][k].state == 1)
						printf("%d %d %d\n", i,j,k);

	exit(0);
}

void check_dimension(int dim, int i, int j, int k, int cube_size){
	
	if(dim == X){//Check x
		if(i==0){
			if(M[cube_size-1][j][k].state == 1)
				M[i][j][k].live_neighbours++;
			if(M[i+1][j][k].state == 1)
				M[i][j][k].live_neighbours++;
		}
		else if(i==cube_size-1){
			if(M[0][j][k].state == 1)
				M[i][j][k].live_neighbours++;
			if(M[i-1][j][k].state == 1)
				M[i][j][k].live_neighbours++;
		}else{
			if(M[i-1][j][k].state == 1)
				M[i][j][k].live_neighbours++;
			if(M[i+1][j][k].state == 1)
				M[i][j][k].live_neighbours++;
		}	
	}

	if(dim == Y){ //Check y
		if(j==0){
			if(M[i][cube_size-1][k].state == 1)
				M[i][j][k].live_neighbours++;
			if(M[i][j+1][k].state == 1)
				M[i][j][k].live_neighbours++;
		}
		else if(j==cube_size-1){
			if(M[i][0][k].state == 1)
				M[i][j][k].live_neighbours++;
			if(M[i][j-1][k].state == 1)
				M[i][j][k].live_neighbours++;
		}else{
			if(M[i][j-1][k].state == 1)
				M[i][j][k].live_neighbours++;
			if(M[i][j+1][k].state == 1)
				M[i][j][k].live_neighbours++;
		}
	}
	
	if(dim == Z){ 	//Check z
		if(k==0){
			if(M[i][j][cube_size-1].state == 1)
				M[i][j][k].live_neighbours++;
			if(M[i][j][k+1].state == 1)
				M[i][j][k].live_neighbours++;
		}
		else if(k==cube_size-1){
			if(M[i][j][0].state == 1)
				M[i][j][k].live_neighbours++;
			if(M[i][j][k-1].state == 1)
				M[i][j][k].live_neighbours++;
		}else{
			if(M[i][j][k-1].state == 1)
				M[i][j][k].live_neighbours++;
			if(M[i][j][k+1].state == 1)
				M[i][j][k].live_neighbours++;
		}	
	}

}