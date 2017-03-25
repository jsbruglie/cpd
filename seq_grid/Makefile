OBJECT_FILES = seq_grid.c list.o 
CFLAGS = -ggdb -Wall 
LIBS = -fopenmp
CC = gcc  

all: seq_grid cleanup

seq_grid: $(OBJECT_FILES)

seq_grid:
	$(CC) $(CFLAGS) -o $@ $(LIBS) $^ 
	
seq_grid.o:

%.o: %.c
	$(CC) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f seq_grid *.o *~ 