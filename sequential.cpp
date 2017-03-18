/** @file sequential.cpp
 *  @brief A sequential implementation of 3D Game Of Life in sparse graphs 
 *
 *  Consists of the sequential implementation of the project that is used as
 *  a baseline when calculating the speedup of the OpenMP and MPI versions.
 *
 *  @author Pedro Abreu
 *  @author João Borrego
 *  @author Miguel Cardoso
 */

#include "sequential.hpp"

int main(int argc, char* argv[]){

    struct timeval start, end;  /**< Timing variables */

    gettimeofday(&start, NULL);
//-------------------------------------------------------------BENCHMARK CODE STARTS HERE----------------------------------------------------

    std::ifstream infile(argv[1]);
    int number_generations = atoi(argv[2]);
    int cube_size;
    
    vector< vector< list <int> > > graph;    /**< The graph representation */
    set<Cell> cell_set;                		 


    parse_file(cube_size, graph, infile, cell_set);
    
    /* TODO - PROGRAM MAIN LOOP
    // For each generation
    for (int g = 1; g <= number_generations; g++){
        // Obtain inactive neighbors and store in a set
        // For node iter in alive
            // temp_array[6][3] = get_neighbor_coordinates(int x,y,z) 
            // for neighbor in temp_array
                // try insertion in dead set
        // Section 1
        // For node iter in alive 
            // alive_array[i] = check_alive(grid, iter);
        // Section 2
        // For neighbor iter in dead
            // dead_array[i] = check_alive(grid, iter);
        // The following two steps both resort to the previous arrays
        // Update graph representation
        // Update the alive list
    }
    return;
    */
    
    for(int g = 1; g <= number_generations; g++){
        cout << "Generation " << g << endl; //DEBUG
        set<Cell>::const_iterator c;
        for(c=cell_set.begin(); c!=cell_set.end(); ++c){

        }
        
        
    }
    //-------------------------------------------------------------BENCHMARK CODE ENDS HERE----------------------------------------------------

    gettimeofday(&end, NULL);
    cout << "Seconds: " << ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6 << endl;
    return 0;
}





// TODO - check_alive
// boolean check_alive(graph, node)
// returns true if n = count_neighbors(graph, node) && n >= 2 && n <= 4 else false

// TODO - count_neighbors(graph, node)
// returns the number of alive neighbors given a node and the graph for efficient search

// TODO - get_neighbor_coordinates(int x,y,z)

// TODO - think of proper function names
Cell* getNeighbours(int x, int y, int z){
	Cell n[6];
	//(!isInList(k, graph[(i+1)%cube_size][j])) ? d[0] = Cell() : live_neighbours++;


}

void check_dimension(int dim, int i, int j, int k, vector<vector<list<int> > > graph, int &live_neighbours, int cube_size){
	switch(dim){
		case X:
			if(isInList(k, graph[(i+1)%cube_size][j])) {  live_neighbours++;  }
			if(isInList(k, graph[((i-1) < 0 ? (cube_size-1) : (i-1))][j])) {  live_neighbours++;  }
			break;
		case Y:
			if(isInList(k, graph[i][(j+1)%cube_size])) {  live_neighbours++;  }
			if(isInList(k, graph[i][((j-1) < 0 ? (cube_size-1) : (j-1))])) {  live_neighbours++;  }
			break;
		case Z:
			if(isInList((k+1)%cube_size, graph[i][j])) {  live_neighbours++;  }
			if(isInList(((k-1) < 0 ? (cube_size-1) : (k-1)), graph[i][j])) {  live_neighbours++;  }
			break;
	}
}

// Auxiliar Functions
// TODO - std::lists already have a built-in find function for ints ...
bool isInList(int k, list<int> graph){
    //If list is empty return false
    if(graph.empty()){
        return false;
    }else{
        list<int>::const_iterator iterator;
        for(iterator = graph.begin(); iterator != graph.end();++iterator){
            if(*iterator == k)
                return true;
        }
    }
    return false; 
}

void parse_file(int &cube_size, vector<vector<list<int> > > &graph, std::ifstream infile, set<int> &cell_set){
	int x,y,z;
	std::string line;
	if(!infile.good()){
        exit(EXIT_FAILURE);
    }

    // Read first line
    getline(infile, line);
    cube_size = atoi(line.data());
    graph.resize(cube_size);
    for(int i=0;i<cube_size;i++)
        graph[i].resize(cube_size);
    
    // Read remaing lines
    while (getline(infile, line)){
        std::istringstream iss(line);
        if ((iss >> x >> y >> z)) { 
            cout << "Read: X " << x << " Y " << y << " Z " << z << endl;
            graph[x][y].push_back(z);
            Cell new_cell(x,y,z);
            cell_set.insert(new_cell);
        }
    }
}

