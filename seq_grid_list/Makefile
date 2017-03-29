OBJECT_FILES = seq_grid_list.o lists.o
CFLAGS = -ggdb -Wall
LIBS = -fopenmp
CC = gcc  

all: seq_grid_list cleanup

seq_grid_list: $(OBJECT_FILES)

seq_grid_list:
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
	
seq_grid_list.o:

%.o: %.c
	$(CC) -c $<  

cleanup:
	$ rm -f *.o

clean:
	$ rm -f seq_grid_list *.o *~ 