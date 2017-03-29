OBJECT_FILES = par_grid_list.o lists.o
CFLAGS = -ggdb -Wall
LIBS =
CC = gcc -fopenmp  

all: par_grid_list cleanup

par_grid_list: $(OBJECT_FILES)

par_grid_list:
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) 
	
par_grid_list.o:

%.o: %.c
	$(CC) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f par_grid_list *.o *~ 