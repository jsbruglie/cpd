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
	int number_generations = atoi(argv[2]);
	FILE* fp = fopen(fileName, "r");
	int first = 0;
	int cube_size;
	char line[6]; //Only need 6 fo x y z
	while(fgets(line, sizeof(line), fp)){ //fgets doesn't strip \n
		if(!first){
			sscanf(line, "%d\n", &cube_size);
			first = 1;
		}
		printf("%s\n", line);


	}








	exit(0);
}