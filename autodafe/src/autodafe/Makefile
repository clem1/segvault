CC = gcc
LIBS = -lz -lxml2 -lutil -lpthread -lm 
CFLAGS = -O2
DEFS = -DHAVE_CONFIG_H
WALL = -Wall
OBJ = debug.o file.o dbg.o output.o chrono.o network.o transmit.o hash.o opcode.o engine.o autodafe.o

all: autodafe

autodafe:	$(OBJ)
	$(CC) $(WALL) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f  *~ *.o autodafe

install:all
	cp ./autodafe /usr/local/bin

	cp -r ../../etc/generator/autodafe /usr/local/etc

uninstall:
	rm /usr/local/bin/autodafe
