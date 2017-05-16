SEQ_OBJECT_FILES = life3d.o lists.o
CFLAGS = -ggdb -Wall
LIBS =
CC = gcc -fopenmp  
FLAG =

all: life3d cleanup

life3d: $(SEQ_OBJECT_FILES)

life3d:
	$(CC) $(CFLAGS) $(FLAG) $^ $(LIBS) -o $@


life3d.o:

%.o: %.c
	$(CC) $(FLAG) -c $<

cleanup:
	rm -f *.o

clean:
	rm -f life3d *.o *~ 
