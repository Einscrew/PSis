CC=gcc
FLAGS=-lpthread
EXTRA=-Wall -g
APP=app
CLIP=clipboard
ALL=$(CLIP) $(APP)


all: $(ALL)

#############################################################
#						 Objects                            #
#############################################################
clipboard: clipboard.c connection.o library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)
		
app: app_teste.c connection.o library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

#############################################################
#						Libraries                           #
#############################################################
connection.o: connection.c connection.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

library.o: library.c clipboard.h connection.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o *.gch $(ALL)