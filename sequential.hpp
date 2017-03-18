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
using std::vector;
using std::set;
using std::list;

#define X 0
#define Y 1
#define Z 2
#define ALIVE true
#define DEAD false


class Cell{
	public:
		int x;
		int y;
		int z;
		bool state;
		bool next_state;
		Cell(int a, int b, int c);
};
Cell::Cell(int a, int b, int c){
	x = a;
	y = b;
	z = c;
	state = ALIVE;
	next_state = ALIVE;
}

bool isInList(int k, std::list<int> grid);
void check_dimension(int dim, int i, int j, int k, std::vector<std::vector<std::list<int> > > grid, int &live_neighbours, int cube_size);
#endif