CC=gcc
LIB=../lib
FLAGS=-lpthread
EXTRA=-Wall -g
ALL =connection.o library.o list.o utils.o

all: $(ALL)
#############################################################
#						Libraries                           #
#############################################################
connection.o: connection.c connection.h utils.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

library.o: library.c clipboard.h connection.h utils.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

list.o: list.c list.h defs.h utils.h
	$(CC) -c $^ $(EXTRA)

utils.o: utils.c utils.h
	$(CC) -c $^ $(EXTRA)

clean:
	rm -f $(ALL) *.gch
