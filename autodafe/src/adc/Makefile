CC = gcc
LIBS = -lz -lxml2 -lutil -lpthread -lm 
FLAGS = -O2
DEFS = -DHAVE_CONFIG_H
WALL = -Wall
OBJ = lexer.o parser.o
LEX = flex
YACC = bison -y

all: adc

.c.o:
	$(CC) -c $<

adc: 	$(OBJ)
	$(CC) $(WALL) $(FLAGS) $^ -o $@

parser.c parser.h: parser.y
	$(YACC) -dtv parser.y
	mv -f y.tab.c parser.c
	mv -f y.tab.h parser.h

lexer.c: lexer.l
	$(LEX) -t lexer.l > lexer.c

lexer.o: parser.h

clean:
	rm -f  *~ *.o adc parser.c parser.h y.output

install: all
	cp ./adc /usr/local/bin

uninstall:
	rm /usr/local/bin/adc
