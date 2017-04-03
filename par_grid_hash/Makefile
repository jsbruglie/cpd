OBJECT_FILES = par_grid_hash.o hash.o hash_lists.o
CFLAGS = -ggdb -Wall
LIBS =
CC = gcc -fopenmp  
FLAG =

all: par_grid_hash cleanup

par_grid_hash: $(OBJECT_FILES)

par_grid_hash:
	$(CC) $(CFLAGS) $(FLAG) -o $@ $^ $(LIBS) 
	
par_grid_hash.o:

%.o: %.c
	$(CC) $(FLAG) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f par_grid_hash *.o *~ 
