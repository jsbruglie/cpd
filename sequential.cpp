#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]){
	std::ifstream infile(argv[1]);
	int x,y,z, cube_size;
	std::string line;
	while (getline(infile, line)){
	    std::istringstream iss(line);
	    if (!(iss >> x >> y >> z)) { 
	    	cout << atoi(cube_size) << endl;
	    	 	
	    }else{
	    	cout << "Read: X " << x << " Y " << y << " Z " << z << endl	;
	    }
	}




	return 0;
}