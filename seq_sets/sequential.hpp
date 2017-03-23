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
using std::string;

#define ALIVE true
#define DEAD false

#define EPSILON 1e-8

class Cell{
    public:
        /* Attributes */
        int x;                      /**< X coordinate */
        int y;                      /**< Y coordinate */
        int z;                      /**< Z coordinate */
        mutable bool state;         /**< Current state of the cell: true for ALIVE, false for DEAD */
        mutable bool next_state;    /**< Next state of the cell */
        /* Methods */
        Cell(int a, int b, int c, bool s);
        /* @brief */
        bool operator<(const Cell& right) const {
            if (x < right.x - EPSILON) return true;
            if (x > right.x + EPSILON) return false;
            if (y < right.y - EPSILON) return true;
            if (y > right.y + EPSILON) return false;
            if (z < right.z - EPSILON) return true;
            return false;
        }
};

/* @brief Cell constructor
 * @param a X coordinate
 * @param b Y coordinate
 * @param c Z coordinate
 * @param s initial state
 */
Cell::Cell(int a, int b, int c, bool s){
    x = a;
    y = b;
    z = c;
    state = s;
    next_state = s;
}

/* @brief checks if a set contains a given integer
 *
 *
 */
bool setContains(int k, std::set<int> grid);

/* @brief
 *
 */
void check_dimension(int dim, int i, int j, int k, std::set<int>*** grid, int &live_neighbours, int cube_size);

/* @brief
 *
 */
Cell* getNeighbours(int x, int y, int z, int cube_size);

/* @brief
 *
 */
void parse_args(int argc, char* argv[], string &file, int &generations);

/* @ brief
 *
 */
std::set<int>*** parse_file(string file, int &cube_size, std::set<Cell> &cell_set);

/* @ brief
 *
 */
bool setNextState(bool st, std::set<int>*** graph, int cube_size, int x, int y, int z);

/* @ brief
 *
 */
int liveNeighbors(int i, int j, int k, std::set<int>*** graph, int cube_size);


void printGraph(set<int>*** graph, int cube_size);

#endif