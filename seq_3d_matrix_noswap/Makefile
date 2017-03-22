OBJECT_FILES = seq_3d_schedule.o 
CFLAGS = -ggdb -Wall
LIBS = -fopenmp
CC = gcc

all: seq_3d_schedule cleanup  
seq_3d_schedule: $(OBJECT_FILES) 

seq_3d_schedule:
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

seq_3d_schedule.o:

%.o: %.c
	$(CC) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f seq_3d_schedule *.o *~ 