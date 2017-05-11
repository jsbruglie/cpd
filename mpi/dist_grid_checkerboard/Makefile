OBJECT_FILES = dist_grid_checkerboard.o graph.o
CFLAGS =
LIBS = -lm
CC = mpicc
FLAG =

all: dist_grid.bin cleanup

dist_grid.bin: $(OBJECT_FILES)

dist_grid.bin:
	$(CC) $(CFLAGS) $(FLAG) $^ $(LIBS) -o $@

dist_grid.o:

graph.o: graph.c
	gcc -c graph.c -o graph.o

dist_grid.o: dist_grid.c
	$(CC) $(FLAG) -c $<

cleanup:
	rm -f *.o

clean:
	rm -f dist_grid.bin graph *.o *~
