OBJECT_FILES = seq_3d.o 
CFLAGS = -ggdb -Wall -fopenmp
LIBS =
CC = gcc  

all: seq_3d cleanup

seq_3d: $(OBJECT_FILES)

seq_3d:
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
	
seq_3d.o:

%.o: %.c
	$(CC) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f seq_3d *.o *~ 