OBJECT_FILES = par_grid_list.o lists.o
CFLAGS = -ggdb -Wall
LIBS =
CC = gcc -fopenmp  
FLAG =

all: par_grid_list cleanup

par_grid_list: $(OBJECT_FILES)

par_grid_list:
	$(CC) $(CFLAGS) $(FLAG) -o $@ $^ $(LIBS) 
	
par_grid_list.o:

%.o: %.c
	$(CC) $(FLAG) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f par_grid_list *.o *~ 
