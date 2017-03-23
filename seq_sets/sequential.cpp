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

    string file;                            /**< Input data file name */
    int generations = 0;                    /**< Number of generations to proccess */
    int cube_size = 0;                      /**< Size of the 3D space */
    
    set<int>*** graph;                      /**< The graph representation */
    set<Cell> cell_set;                     /**< The set of cells to be checked */                       

    parse_args(argc, argv, file, generations);
    
    double start = omp_get_wtime();  //Start Timer

    graph = parse_file(file, cube_size, cell_set);
    
    set<Cell>::const_iterator i;
    for(int g = 1; g <= generations; g++){
        // cout << "Generation " << g << endl; //DEBUG
        double g_start = omp_get_wtime();
        /* Make a copy of the live cells set */
        set<Cell> live = cell_set;
        /* Iterate over the temporary copy of live cells set and add (dead) neighbour cells to the set */
        for(i = live.begin(); i != live.end(); ++i){
            //cout << "x:" << i->x << " y:" << i->y << " z:" << i-> z << " state:" << i->state << " next:" << i->next_state << endl;
            Cell* neighbours = getNeighbours(i->x, i->y, i->z, cube_size);
            for (int j = 0; j < 6; ++j){
                cell_set.insert((Cell) neighbours[j]);
            }
            free(neighbours);
        }
        
        double g_neighbors = omp_get_wtime();
        cout << "Done adding cells to the set. Took: " << g_neighbors - g_start << endl;

        /* Check each cell and update its next state field */
        for(i = cell_set.begin(); i != cell_set.end(); ++i){
            i->next_state = setNextState(i->state, graph, cube_size, i->x, i->y, i->z);
        }

        double g_state = omp_get_wtime();
        cout << "Done processing states. Took: " << g_state - g_neighbors << endl;

        /* Update Graph */
        for(i = cell_set.begin(); i != cell_set.end(); ++i){
            if(i->state != i->next_state){
                // If a live cell is scheduled to die 
                if(i->state == ALIVE){
                    (*graph[i->x][i->y]).erase(i->z);
                }
                // If a dead cell is scheduled to live   
                else if(i->state == DEAD){
                    (*graph[i->x][i->y]).insert(i->z);
                }
                i->state = i->next_state;
            }    
        }
        /* Remove Dead Cells from cell_set */
        for(i = cell_set.begin(); i != cell_set.end(); ++i){
            if(i->state == DEAD)
                cell_set.erase(*i);    
        }

        double g_end = omp_get_wtime();
        cout << "Done updating graph. Took "<< g_end - g_state << endl << "Iteration took " << g_end - g_start << endl; 
    }

    /* Print the final set of live cells */
    /*for(i = cell_set.begin(); i != cell_set.end(); ++i){
        cout << i->x << " " << i->y << " " << i-> z << endl;    
    }*/

    double end = omp_get_wtime();   // Stop Timer
    cout << "Total Runtime: " << (end - start) << endl;
    return 0;
}

bool setNextState(bool state, set<int>*** graph, int cube_size, int x, int y, int z){
    int live_neighbours = liveNeighbors(x, y, z, graph, cube_size);
    //cout << "x:" << x << " y:" << y << " z:" << z << " state:" << state << " live_neighbours:" << live_neighbours; // DEBUG
    if (state == ALIVE){
        if(live_neighbours < 2 || live_neighbours > 4){
            //cout << " dies." << endl; // DEBUG
            return DEAD;
        }
    }else if(state == DEAD){
        if(live_neighbours == 2 || live_neighbours == 3){
            //cout << " lives." << endl; // DEBUG
            return ALIVE;
        }   
    }
    //cout << " unchanged." << endl; // DEBUG
    return state;
}

Cell* getNeighbours(int x, int y, int z, int cube_size){
    
    Cell* neighbours = (Cell*) malloc(sizeof(Cell) * 6);
    neighbours[0] = Cell((x+1)%cube_size,y,z,DEAD);
    neighbours[1] = Cell((x-1) < 0 ? (cube_size-1) : (x-1),y,z,DEAD);
    neighbours[2] = Cell(x,(y+1)%cube_size,z,DEAD);
    neighbours[3] = Cell(x,((y-1) < 0 ? (cube_size-1) : (y-1)),z,DEAD);
    neighbours[4] = Cell(x,y,(z+1)%cube_size,DEAD);
    neighbours[5] = Cell(x,y,((z-1) < 0 ? (cube_size-1) : (z-1)),DEAD);
    return neighbours;
}

int liveNeighbors(int i, int j, int k, set<int>*** graph, int cube_size){
    int live_neighbours = 0;
    if(setContains(k, *graph[(i+1)%cube_size][j])) {  live_neighbours++;  }
    if(setContains(k, *graph[((i-1) < 0 ? (cube_size-1) : (i-1))][j])) {  live_neighbours++;  }
    if(setContains(k, *graph[i][(j+1)%cube_size])) {  live_neighbours++;  }
    if(setContains(k, *graph[i][((j-1) < 0 ? (cube_size-1) : (j-1))])) {  live_neighbours++;  }
    if(setContains((k+1)%cube_size, *graph[i][j])) {  live_neighbours++;  }
    if(setContains(((k-1) < 0 ? (cube_size-1) : (k-1)), *graph[i][j])) {  live_neighbours++;  }
    return live_neighbours;
}

bool setContains(int k, std::set<int> _set){
    if (_set.count(k)){
        return true;
    }
    return false;
}


void printGraph(set<int>*** graph, int cube_size){
    cout << "Printing graph" << endl;
    for (int x = 0; x < cube_size; x++){
        for (int y = 0; y < cube_size; y++){
            set<int>* tmp = graph[x][y];
            if (tmp != NULL){
                for (set<int>::const_iterator i = tmp->begin(); i != tmp->end(); ++i){
                    cout << "x:" << x << " y:" << y << " z:" << (int) *i << endl;
                }
            }
        }
    }
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

set<int>*** parse_file(string file, int &cube_size, set<Cell> &cell_set){
    
    int x,y,z;
    set<int>*** graph;
    std::ifstream infile(file.c_str());

    std::string line;
    if(!infile.good()){
        cout << "ERROR: Could not find file." << endl;
        exit(EXIT_FAILURE);
    }

    // Read first line
    getline(infile, line);
    cube_size = atoi(line.data());
    
    graph = (set<int>***) malloc(sizeof(set<int>**) * cube_size);
    
    for(int i = 0; i < cube_size; i++){
        graph[i] = (set<int>**) malloc(sizeof(set<int>*) * cube_size);
        for (int j = 0; j < cube_size; j++){
            graph[i][j] = new set<int>;
        }
    }       

    // Read remaing lines
    while (getline(infile, line)){
        std::istringstream iss(line);
        if ((iss >> x >> y >> z)) { 
            //cout << "Read: x:" << x << " y:" << y << " z:" << z << endl;
            graph[x][y]->insert(z);
            Cell new_cell(x,y,z,ALIVE);
            cell_set.insert(new_cell);
        }
    }
    return graph;
}