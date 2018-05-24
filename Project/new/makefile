CC=gcc
FLAGS=-lpthread
EXTRA=-Wall -g
APP=app app_test
CLIP=clipboard
ALL=$(CLIP) $(APP)


all: $(ALL)

#############################################################
#						 Objects                            #
#############################################################
clipboard: clipboard.c connection.o library.o list.o utils.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)
		
app_test: app_teste.c connection.o library.o utils.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

app: app.c connection.o library.o utils.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

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
	rm -f *.*o *.gch $(ALL)

cdir:
	rm -fR -- */

c: clean cdir


#echo -e '\x0\x1\x0\x0\x432e8o2uhd aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaawydgawdawdawdawdawdawdawjiiiiiiiiiiiiiiiiiiiiiiiiis\x4\x0\x0\x0\x43231' | nc -U /home/einstein/IST/MSc/PSis/repo/Project/new/clipSocket
