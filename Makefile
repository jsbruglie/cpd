TEST_OBJECT_FILES = test.o 
SEQUENTIAL_OBJECT_FILES = sequential.o
CFLAGS = -ggdb -Wall
LIBS =
CC = gcc  
CXX = g++
CXXFLAGS = -Wall
CXXLIBS = -fopenmp


all: test sequential cleanup  
test: $(TEST_OBJECT_FILES) 

test:
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

test.o: 

sequential: $(SEQUENTIAL_OBJECT_FILES)
	
sequential:
	$(CXX) $(CXXFLAGS) $^ $(CXXLIBS) -o $@

sequential.o:

%.o: %.c
	$(CC) -c $<  

%.o: %.cpp
	$(CXX) -c $<

cleanup:
	rm -f *.o

clean:
	rm -f test sequential *.o *~ 