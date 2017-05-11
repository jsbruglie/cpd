OBJECT_FILES = dist_grid.o graph.o
CFLAGS =
LIBS =
CC = mpicc
FLAG =

all: dist_grid cleanup

dist_grid: $(OBJECT_FILES)

dist_grid:
	$(CC) $(CFLAGS) $(FLAG) $^ $(LIBS) -o $@

dist_grid.o:

graph.o: graph.c
	gcc -c graph.c -o graph.o

dist_grid.o: dist_grid.c
	$(CC) $(FLAG) -c $<

cleanup:
	rm -f *.o

clean:
	rm -f dist_grid graph *.o *~
