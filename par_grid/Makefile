OBJECT_FILES = par_grid.o lists.o
CFLAGS = -ggdb -Wall
LIBS =
CC = gcc -fopenmp  

all: par_grid cleanup

par_grid: $(OBJECT_FILES)

par_grid:
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
	
par_grid.o:

%.o: %.c
	$(CC) -c $<  

cleanup:
	$ rm -f *.o

clean:
	$ rm -f par_grid *.o *~ 