OBJECT_FILES = dist_grid_rows.o graph.o
CFLAGS =
LIBS =
CC = mpicc
FLAG =

all: dist_grid_rows cleanup

dist_grid_rows: $(OBJECT_FILES)

dist_grid_rows:
	$(CC) $(CFLAGS) $(FLAG) $^ $(LIBS) -o $@

dist_grid_rows.o:

graph.o: graph.c
	gcc -c graph.c -o graph.o

dist_grid_rows.o: dist_grid_rows.c
	$(CC) $(FLAG) -c $<

cleanup:
	rm -f *.o

clean:
	rm -f dist_grid_rows graph *.o *~
