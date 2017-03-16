#include "sequential.hpp"
using namespace std;
using std::vector;
using std::list;

int main(int argc, char* argv[]){ //g++ -fopenmp -o seq sequential.cpp

	struct timeval start, end;
	gettimeofday(&start, NULL);
//-------------------------------------------------------------BENCHMARK CODE STARTS HERE----------------------------------------------------
	omp_set_num_threads(8);
	std::ifstream infile(argv[1]);
	int number_generations = atoi(argv[2]);
	int x,y,z, cube_size, live_neighbours;
	std::string line;
	bool alive;
	
	vector<vector<list<int> > > grid;
	list<Cell> die_schedule;
	list<Cell> born_schedule;

	//Reads first line
	if(infile.good()){
		getline(infile, line);
		cube_size = atoi(line.data());
		grid.resize(cube_size);
		for(int i=0;i<cube_size;i++)
			grid[i].resize(cube_size);

	}
	//Reads all further lines
	while (getline(infile, line)){
		std::istringstream iss(line);
	    if ((iss >> x >> y >> z)) { 
	    	cout << "Read: X " << x << " Y " << y << " Z " << z << endl;
	    	grid[x][y].push_back(z);
	    }
		
	}
	//DEBUG
	//for(int i = 0; i<cube_size;i++){
	//	for(int j=0; j < cube_size; j++){
	//		if(!grid[x][y].empty())
	//			cout << grid[i][j].size() << endl;
	//	}
	//}

	for(int g=0;g<number_generations;g++){
		cout << "Generation " << g+1 << endl;
		for(int i=0; i < cube_size; i++){
			for(int j=0; j<cube_size;j++){
				for(int k=0; k<cube_size;k++){
					//Go along the "list" to see if we are dealing with a dead or alive cell
					alive = false;
					list<int>::const_iterator iterator;
					for(iterator = grid[i][j].begin(); iterator != grid[i][j].end();++iterator){
						if(*iterator == k)
							alive = true;
					}
					live_neighbours = 0;
					//compute their live neighbours
					check_dimension(X,i,j,k,grid,live_neighbours, cube_size);
					check_dimension(Y,i,j,k,grid,live_neighbours, cube_size);
					check_dimension(Z,i,j,k,grid,live_neighbours, cube_size);
					//If its a live cell
					if(alive){
						if(live_neighbours < 2 || live_neighbours > 4){
							Cell new_cell(i,j,k);
							die_schedule.push_back(new_cell);
						}
						//Check and schedule them to die
					}else{
						//If its a dead
						//Schedule them to be born
						if(live_neighbours==2 || live_neighbours==3){
							Cell new_cell(i,j,k);
							born_schedule.push_back(new_cell);
						}
					}
				}
			}
		}
		//Remove the ones scheduled to die
		list<Cell>::const_iterator iterator;
		for(iterator = die_schedule.begin(); iterator != die_schedule.end();++iterator){
			Cell c = *iterator;
			grid[c.x][c.y].remove(c.z);
		}
		//Insert the ones scheduled to be born
		for(iterator = born_schedule.begin(); iterator != born_schedule.end();++iterator){
			Cell c = *iterator;
			grid[c.x][c.y].push_back(c.z);
		}
 		for(int i=0;i<cube_size;i++) //X
			for(int j=0;j<cube_size;j++) //Y
				if(!grid[i][j].empty()){
					list<int>::const_iterator iterator;
					for(iterator = grid[i][j].begin(); iterator != grid[i][j].end();++iterator){
						cout << i << " " << j << " " << *iterator << endl;
					}
				}
	
	    die_schedule.clear();
	    born_schedule.clear();
	}
	//-------------------------------------------------------------BENCHMARK CODE ENDS HERE----------------------------------------------------
	gettimeofday(&end, NULL);
	cout << "Seconds: " << ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6 << endl;
	return 0;
}


bool isInList(int k, list<int> grid){
	//If list is empty return false
	if(grid.empty()){
		return false;
	}else{
		list<int>::const_iterator iterator;
		for(iterator = grid.begin(); iterator != grid.end();++iterator){
			if(*iterator == k)
				return true;
		}
	}
	return false; //Couldn't find it
}

void check_dimension(int dim, int i, int j, int k, vector<vector<list<int> > > grid, int &live_neighbours, int cube_size){
	if(dim == X){
		if(i==0){
			if(isInList(k,grid[cube_size-1][j]))
				live_neighbours++;
			if(isInList(k,grid[i+1][j]))
				live_neighbours++;
		}else if(i==cube_size-1){
			if(isInList(k,grid[0][j]))
				live_neighbours++;
			if(isInList(k,grid[i-1][j]))
				live_neighbours++;
		}else{
			if(isInList(k,grid[i-1][j]))
				live_neighbours++;
			if(isInList(k,grid[i+1][j]))
				live_neighbours++;
		}	
	}
	if(dim == Y){
		if(j==0){
			if(isInList(k,grid[i][cube_size-1]))
				live_neighbours++;
			if(isInList(k,grid[i][j+1]))
				live_neighbours++;
		}else if(j==cube_size-1){
			if(isInList(k,grid[i][0]))
				live_neighbours++;
			if(isInList(k,grid[i][j-1]))
				live_neighbours++;
		}else{
			if(isInList(k,grid[i][j-1]))
				live_neighbours++;
			if(isInList(k,grid[i][j+1]))
				live_neighbours++;
		}
	}
	//This is diferent now, its only going along the list
	if(dim == Z){
		if(k==0){
			if(isInList(cube_size-1,grid[i][j]))
				live_neighbours++;
			if(isInList(k+1,grid[i][j]))
				live_neighbours++;
		}else if(k==cube_size-1){
			if(isInList(0, grid[i][j]))
				live_neighbours++;
			if(isInList(k-1, grid[i][j]))
				live_neighbours++;
		}else{
			if(isInList(k-1, grid[i][j]))
				live_neighbours++;
		}
	}
}