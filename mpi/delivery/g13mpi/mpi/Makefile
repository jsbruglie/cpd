OBJECT_FILES = life3d-mpi.o graph.o
CFLAGS =
LIBS = -lm
CC = mpicc
FLAG =

all: life3d-mpi.bin cleanup

life3d-mpi.bin: $(OBJECT_FILES)

life3d-mpi.bin:
	$(CC) $(CFLAGS) $(FLAG) $^ $(LIBS) -o $@

life3d-mpi.o:

graph.o: graph.c
	gcc -c graph.c -o graph.o

life3d-mpi.o: life3d-mpi.c
	$(CC) $(FLAG) -c $<

cleanup:
	rm -f *.o

clean:
	rm -f life3d-mpi.bin graph *.o *~
