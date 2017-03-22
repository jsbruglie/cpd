/** @file seq_3d.cpp
 *  @brief 3D Matrix with duplicate for update
 *  @author Pedro Abreu
 *  @author João Borrego
 *  @author Miguel Cardoso
 */

#include "seq_3d_cpp.hpp"

int main(int argc, char* argv[]){

    string file;                                /**< Input data file name */
    int generations = 0;                        /**< Number of generations to proccess */
    int cube_size = 0;                          /**< Size of the 3D space */
    
    vector< vector< vector <bool> > > graph0;   /**< The graph representation */
    vector< vector< vector <bool> > > graph1;   /**< A copy of the graph */                       

    parse_args(argc, argv, file, generations);
    cout << "ARGS: file: " << file << " generations: " << generations << endl;

    double start = omp_get_wtime();  //Start Timer

    parse_file(file, cube_size, graph0);
    graph1 = graph0;

    for(int g = 1; g <= generations; g++){
        
        // cout << "Generation " << g << endl; //DEBUG

        /* Check each node */
        for (int x = 0; x < cube_size; ++x){
            for (int y = 0; y < cube_size; ++y){
                for (int z = 0; z < cube_size; ++z){
                    graph1[x][y][z] = setNextState(graph0, cube_size, x, y, z);
                }
            }
        }
        //std::swap(graph0, graph1);
    }

    /* Print the final set of live cells */
    printActive(graph1, cube_size);

    double end = omp_get_wtime();   // Stop Timer
    cout << "Total Runtime: " << (end - start) << endl;
    
    return 0;
}

vector<vector<vector<bool> > > initGraph(int size){

    vector<vector<vector<bool> > > graph (size,vector<vector<bool> >(size,vector <bool>(size,DEAD)));
    return graph;
}

void printActive(vector<vector<vector<bool> > > graph, int cube_size){
    for (int x = 0; x < cube_size; ++x){
        for (int y = 0; y < cube_size; ++y){
            for (int z = 0; z < cube_size; ++z){
                if (graph[x][y][z])
                    cout << "x:" << x << " y:" << y <<" z:" << z << endl;
            }
        }
    }
}

bool setNextState(vector<vector<vector<bool> > > graph, int cube_size, int x, int y, int z){
    int live_neighbours = liveNeighbors(graph, cube_size, x, y, z);
    //cout << "x:" << x << " y:" << y << " z:" << z << " state:" << state << " live_neighbours:" << live_neighbours; // DEBUG
    bool state = graph[x][y][z];
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

int liveNeighbors(vector<vector<vector<bool> > > graph, int cube_size, int x, int y, int z){
    int live_neighbours = 0;
    int x1,x2,y1,y2,z1,z2;

    x1 = (x+1)%cube_size; x2 = (x-1) < 0 ? (cube_size-1) : (x-1);
    y1 = (y+1)%cube_size; y2 = (y-1) < 0 ? (cube_size-1) : (y-1);
    z1 = (z+1)%cube_size; z2 = (z-1) < 0 ? (cube_size-1) : (z-1);

    if(graph[x1][y][z]){live_neighbours++;}
    if(graph[x2][y][z]){live_neighbours++;}
    if(graph[x][y1][z]){live_neighbours++;}
    if(graph[x][y2][z]){live_neighbours++;}
    if(graph[x][y][z1]){live_neighbours++;}
    if(graph[x][y][z2]){live_neighbours++;}

    return live_neighbours;
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

void parse_file(string file, int &cube_size, vector<vector<vector<bool> > > &graph){
    
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

    graph = initGraph(cube_size); 

    // Read remaing lines
    while (getline(infile, line)){
        std::istringstream iss(line);
        if ((iss >> x >> y >> z)) { 
            //cout << "Read: x " << x << " y " << y << " z " << z << endl;
            graph[x][y][z] = ALIVE;
        }
    }
}