CC = gcc
LIBS = -lz -lxml2 -lutil -lpthread -lm 
CFLAGS = -O2
DEFS = -DHAVE_CONFIG_H
WALL = -Wall
OBJ = debug.o gdb.o network.o adbg.o

all: adbg

adbg:	$(OBJ)
	$(CC) $(WALL) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f *~ *.o adbg

install:all
	cp ./adbg /usr/local/bin

uninstall:
	rm /usr/local/bin/adbg
