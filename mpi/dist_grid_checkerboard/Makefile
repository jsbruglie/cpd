OBJECT_FILES = dist_grid_checkerboard.o graph.o
CFLAGS =
LIBS = -lm
CC = mpicc
FLAG =

all: dist_grid_checkerboard.bin cleanup

dist_grid_checkerboard.bin: $(OBJECT_FILES)

dist_grid_checkerboard.bin:
	$(CC) $(CFLAGS) $(FLAG) $^ $(LIBS) -o $@

dist_grid_checkerboard.o:

graph.o: graph.c
	gcc -c graph.c -o graph.o

dist_grid_checkerboard.o: dist_grid_checkerboard.c
	$(CC) $(FLAG) -c $<

cleanup:
	rm -f *.o

clean:
	rm -f dist_grid_checkerboard graph *.o *~
