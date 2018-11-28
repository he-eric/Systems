CFLAGS = -fsanitize=address -pthread

libnetfiles.o: libnetfiles.c libnetfiles.h
	gcc $(CFLAGS) -c libnetfiles.c -lm

#libnetfiles:
#	gcc libnetfiles.c -o libnetfiles -lm 

netfileserver: netfileserver.c
	gcc $(CFLAGS) netfileserver.c -o netfileserver -lm

clean:
	rm -rf libnetfiles.o
	rm -rf netfileserver

