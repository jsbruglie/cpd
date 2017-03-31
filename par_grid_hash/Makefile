OBJECT_FILES = par_grid_hash.o hash.o lists.o
CFLAGS = -ggdb -Wall
LIBS =
CC = gcc -fopenmp  

all: par_grid_hash cleanup

par_grid_hash: $(OBJECT_FILES)

par_grid_hash:
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) 
	
par_grid_hash.o:

%.o: %.c
	$(CC) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f par_grid_hash *.o *~ 