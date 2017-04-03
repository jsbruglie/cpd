OBJECT_FILES = seq_grid.o lists.o
CFLAGS = -ggdb -Wall
LIBS =
CC = gcc -fopenmp  
FLAG =

all: seq_grid cleanup

seq_grid: $(OBJECT_FILES)

seq_grid:
	$(CC) $(CFLAGS) $(FLAG) $^ $(LIBS) -o $@
	
seq_grid.o:

%.o: %.c
	$(CC) $(FLAG) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f seq_grid *.o *~ 
