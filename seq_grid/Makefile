OBJECT_FILES = seq_grid.c 
CFLAGS = -ggdb -Wall -fopenmp
LIBS =
CC = gcc  

all: seq_grid cleanup

seq_grid: $(OBJECT_FILES)

seq_grid:
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
	
seq_grid.o:

%.o: %.c
	$(CC) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f seq_grid *.o *~ 