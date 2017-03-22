SEQUENTIAL_OBJECT_FILES = sequential.o
CXX = g++
CXXFLAGS = -Wall
CXXLIBS = -fopenmp


all: sequential cleanup  

sequential: $(SEQUENTIAL_OBJECT_FILES)
	
sequential:
	$(CXX) $(CXXFLAGS) $^ $(CXXLIBS) -o $@

sequential.o:

%.o: %.cpp
	$(CXX) -c $<

cleanup:
	rm -f *.o

clean:
	rm -f test sequential *.o *~ 