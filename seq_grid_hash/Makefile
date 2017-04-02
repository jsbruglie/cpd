OBJECT_FILES = seq_grid_hash.o hash.o lists.o
CFLAGS = -ggdb -Wall
LIBS =
CC = gcc -fopenmp  

all: seq_grid_hash cleanup

seq_grid_hash: $(OBJECT_FILES)

seq_grid_hash:
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) 
	
seq_grid_hash.o:

%.o: %.c
	$(CC) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f seq_grid_hash *.o *~ 