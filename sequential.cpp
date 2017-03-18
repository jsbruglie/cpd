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

    string file;                            /**< Input data file name */
    int generations = 0;                    /**< Number of generations to proccess */
    int cube_size = 0;                      /**< Size of the 3D space */
    
    vector< vector< set <int> > > graph;    /**< The graph representation */
    set<Cell> cell_set;                     /**< The set of cells to be checked */                       

    parse_args(argc, argv, file, generations);
    
    gettimeofday(&start, NULL); // Start timer

    parse_file(file, cube_size, graph, cell_set);

    for(int g = 1; g <= generations; g++){
        cout << "Generation " << g << endl; //DEBUG
        
        set<Cell>::const_iterator i;
        /* Make a copy of the live cells set */
        set<Cell> live = cell_set;
        /* Iterate over the temporary copy of live cells set and add (dead) neighbour cells to the set */
        for(i = live.begin(); i != live.end(); ++i){
            cout << i->x << " " << i->y << " " << i-> z << endl; // DEBUG
            vector<Cell> neighbours = getNeighbours(i->x, i->y, i->z, cube_size);
            vector<Cell>::const_iterator j;
            for (j = neighbours.begin(); j != neighbours.end(); ++j){
                cell_set.insert((Cell)*j);
            }
        }
    //     /* Check each cell and update its next state field */
    //     for(i = cell_set.begin(); i != cell_set.end(); ++i){
    //         //setNextState();    
    //     }
    //     /* Update Graph */
    //     /* Remove Dead Cells from cell_set */
    }

    gettimeofday(&end, NULL);   // Stop Timer
    cout << "Total Runtime: " << ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6 << endl;
    return 0;
}

void setNextState(){

}

vector<Cell> getNeighbours(int x, int y, int z, int cube_size){
    
    std::vector<Cell> neighbours;
    // Cells are assumed to be dead since 
    neighbours.push_back(Cell((x+1)%cube_size,y,z,DEAD));
    neighbours.push_back(Cell((x-1) < 0 ? (cube_size-1) : (x-1),y,z,DEAD));
    neighbours.push_back(Cell(x,(y+1)%cube_size,z,DEAD));
    neighbours.push_back(Cell(x,((y-1) < 0 ? (cube_size-1) : (y-1)),z,DEAD));
    neighbours.push_back(Cell(x,y,(z+1)%cube_size,DEAD));
    neighbours.push_back(Cell(x,y,((z-1) < 0 ? (cube_size-1) : (z-1)),DEAD));
    return neighbours;
}

int liveNeighbors(int i, int j, int k, vector<vector<set<int> > > graph, int cube_size){
    int live_neighbours = 0;
    if(setContains(k, graph[(i+1)%cube_size][j])) {  live_neighbours++;  }
    if(setContains(k, graph[((i-1) < 0 ? (cube_size-1) : (i-1))][j])) {  live_neighbours++;  }
    if(setContains(k, graph[i][(j+1)%cube_size])) {  live_neighbours++;  }
    if(setContains(k, graph[i][((j-1) < 0 ? (cube_size-1) : (j-1))])) {  live_neighbours++;  }
    if(setContains((k+1)%cube_size, graph[i][j])) {  live_neighbours++;  }
    if(setContains(((k-1) < 0 ? (cube_size-1) : (k-1)), graph[i][j])) {  live_neighbours++;  }
    return live_neighbours;
}

bool setContains(int k, std::set<int> _set){
    if (_set.count(k)){
        return true;
    }
    return false;
}

void parse_args(int argc, char* argv[], string &file, int &generations){
    if (argc == 3){
        file = argv[1];
        generations = atoi(argv[2]);
        if (generations > 0 && !file.empty())
            return;
    }    
    cout << "Usage: " << argv[0] << " [data_file.in] [number_generations]" << endl;
    exit(EXIT_FAILURE);
}

void parse_file(string file, int &cube_size, vector<vector<set<int> > > &graph, set<Cell> &cell_set){
    
    int x,y,z;
    std::ifstream infile(file.c_str());

    std::string line;
    if(!infile.good()){
        cout << "ERROR: Could not find file." << endl;
        exit(EXIT_FAILURE);
    }

    // Read first line
    getline(infile, line);
    cube_size = atoi(line.data());
    graph.resize(cube_size);
    for(int i = 0; i < cube_size; i++)
        graph[i].resize(cube_size);
    
    // Read remaing lines
    while (getline(infile, line)){
        std::istringstream iss(line);
        if ((iss >> x >> y >> z)) { 
            cout << "Read: x " << x << " y " << y << " z " << z << endl;
            graph[x][y].insert(z);
            Cell new_cell(x,y,z,ALIVE);
            cell_set.insert(new_cell);
        }
    }
}