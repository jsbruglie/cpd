SEQ_OBJECT_FILES = sequential.o 

CFLAGS = -ggdb -Wall
LIBS =
CC = gcc  

all: sequential cleanup  
sequential: $(SEQ_OBJECT_FILES) 

sequential:
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

sequential.o: 

%.o: %.c
	$(CC) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f sequential *.o *~ 