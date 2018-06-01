CC=gcc
LIB=../lib
FLAGS=-lpthread
EXTRA=-Wall -g
CLIP=clipboard
ALL=$(CLIP)

all: $(ALL)

#############################################################
#						 Objects                            #
#############################################################
clipboard: clipboard.c $(LIB)/connection.o $(LIB)/library.o $(LIB)/list.o $(LIB)/utils.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

#############################################################
#						Libraries                           #
#############################################################
clean:
	rm -f $(ALL)

cdir:
	rm -fR -- */
