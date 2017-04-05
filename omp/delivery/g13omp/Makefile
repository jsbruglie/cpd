SEQ_OBJECT_FILES = life3d.o lists.o
PAR_OBJECT_FILES = life3d-omp.o lists-omp.o
CFLAGS = -ggdb -Wall
LIBS =
CC = gcc -fopenmp  
FLAG =

all: life3d life3d-omp cleanup

life3d: $(SEQ_OBJECT_FILES)
life3d-omp: $(PAR_OBJECT_FILES)

life3d:
	$(CC) $(CFLAGS) $(FLAG) $^ $(LIBS) -o $@

life3d-omp:
	$(CC) $(CFLAGS) $(FLAG) $^ $(LIBS) -o $@

life3d.o:

%.o: %.c
	$(CC) $(FLAG) -c $<

life3d-omp.o:

%.o: %.c
	$(CC) $(FLAG) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f life3d life3d-omp *.o *~ 
