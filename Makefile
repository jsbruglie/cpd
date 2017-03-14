SEQ_OBJECT_FILES = matrix.o 

CFLAGS = -ggdb -Wall
LIBS =
CC = gcc  

all: matrix cleanup  
matrix: $(SEQ_OBJECT_FILES) 

matrix:
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

matrix.o: 

%.o: %.c
	$(CC) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f matrix *.o *~ 