OBJECT_FILES = seq_grid_hash.o seq_hash.o seq_hash_lists.o
CFLAGS = -ggdb -Wall
LIBS =
CC = gcc -fopenmp  
FLAG =

all: seq_grid_hash cleanup

seq_grid_hash: $(OBJECT_FILES)

seq_grid_hash:
	$(CC) $(CFLAGS) $(FLAG) -o $@ $^ $(LIBS) 
	
seq_grid_hash.o:

%.o: %.c
	$(CC) $(FLAG) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f seq_grid_hash *.o *~ 
