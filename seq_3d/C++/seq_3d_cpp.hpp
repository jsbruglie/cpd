#ifndef SEQUENTIAL_HPP
#define SEQUENTIAL_HPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <list>
#include <set>
#include <sys/time.h>
#include <omp.h>

using namespace std;
using std::string;
using std::vector;

#define ALIVE true
#define DEAD false

vector<vector<vector<bool> > > initGraph(int size);

void printActive(vector<vector<vector<bool> > > graph, int cube_size);

/* @brief
 *
 */
bool setNextState(vector<vector<vector<bool> > > graph, int cube_size, int x, int y, int z);

/* @brief
 *
 */
int liveNeighbors(vector<vector<vector<bool> > > graph, int cube_size, int x, int y, int z);

/* @brief
 *
 */
void parse_args(int argc, char* argv[], string &file, int &generations);

/* @ brief
 *
 */
void parse_file(string file, int &cube_size, vector<vector<vector<bool> > > &graph);
    

#endif