OBJECT_FILES = dist_grid.o 
CFLAGS =
LIBS =
CC = mpicc
FLAG =

all: dist_grid cleanup

dist_grid: $(OBJECT_FILES)

dist_grid:
	$(CC) $(CFLAGS) $(FLAG) $^ $(LIBS) -o $@
	
dist_grid.o:

%.o: %.c
	$(CC) $(FLAG) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f dist_grid *.o *~ 
