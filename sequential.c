#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _cell{
	int x;
	int y;
	int z;
}cell;


int main(int argc, char* argv[]){
	char* fileName = argv[1];
	FILE* fp = fopen(fileName, "r");

	int number_generations = atoi(argv[2]);

	int first = 0;
	int cube_size;
	char line[7]; //Only need 7 fo x y z
	int x, y, z;

	while(fgets(line, sizeof(line), fp)){ //fgets doesn't strip \n
		if(!first){
			sscanf(line, "%d\n", &cube_size);
			first = 1;
		}else{
			sscanf(line, "%d %d %d\n", &x, &y, &z);
			printf("%d %d %d\n", x, y, z);
		}
		


	}


	exit(0);
}