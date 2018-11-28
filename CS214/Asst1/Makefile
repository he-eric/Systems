CFLAGS = -Wall -Werror -fsanitize=address
OBJECTS = memgrind.o mymalloc.o
CC = gcc

memgrind: $(OBJECTS)
	CC -o memgrind $(CFLAGS) $(OBJECTS)

memgrind.o: mymalloc.h

mymalloc.o: mymalloc.h

clean:
	rm -f memgrind $(OBJECTS)