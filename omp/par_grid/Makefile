OBJECT_FILES = par_grid.o lists.o
CFLAGS = -ggdb -Wall
LIBS =
CC = gcc -fopenmp
FLAG =

all: par_grid cleanup

par_grid: $(OBJECT_FILES)

par_grid:
	$(CC) $(CFLAGS) $(FLAG) $^ $(LIBS) -o $@
	
par_grid.o:

%.o: %.c
	$(CC) $(FLAG) -c $<  

cleanup:
	$ rm -f *.o

clean:
	$ rm -f par_grid *.o *~ 
