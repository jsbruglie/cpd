SEQ_OBJECT_FILES = test.o 

CFLAGS = -ggdb -Wall
LIBS =
CC = gcc  

all: test cleanup  
test: $(SEQ_OBJECT_FILES) 

test:
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

test.o: 

%.o: %.c
	$(CC) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f test *.o *~ 