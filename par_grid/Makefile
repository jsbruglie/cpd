OBJECT_FILES = par_grid_.o lists.o
CFLAGS = -ggdb -Wall
LIBS =
CC = gcc -fopenmp  

all: par_grid_ cleanup

par_grid_: $(OBJECT_FILES)

par_grid_:
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
	
par_grid_.o:

%.o: %.c
	$(CC) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f par_grid_ *.o *~ 