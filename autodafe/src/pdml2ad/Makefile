CC = gcc
LIBS = -lz -lxml2 -lutil -lpthread -lm  -L /usr/lib
CFLAGS = -O2 -I /usr/include/libxml2
DEFS = -DHAVE_CONFIG_H
WALL = -Wall
OBJ = debug.o output.o recover.o xml.o pdml2ad.o

all: pdml2ad

pdml2ad:	$(OBJ)
	$(CC) $(WALL) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f *~ *.o pdml2ad

install:all
	cp ./pdml2ad /usr/local/bin

uninstall:
	rm /usr/local/bin/pdml2ad
