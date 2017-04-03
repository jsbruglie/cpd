OBJECT_FILES = seq_3d_cpp.o
CXX = g++
CXXFLAGS = -Wall
CXXLIBS = -fopenmp


all: seq_3d_cpp  

seq_3d_cpp: $(OBJECT_FILES)

seq_3d_cpp:
	$(CXX) $(CXXFLAGS) $^ $(CXXLIBS) -o $@

seq_3d_cpp.o:

%.o: %.cpp
	$(CXX) -c $<

cleanup:
	rm -f *.o

clean:
	rm -f test seq_3d_cpp *.o *~ 
