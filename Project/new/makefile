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

#echo -e '\x0\x1\x0\x0\x432e8o2uhd aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaawydgawdawdawdawdawdawdawjiiiiiiiiiiiiiiiiiiiiiiiiis\x4\x0\x0\x0\x43231' | nc -U /home/einstein/IST/MSc/PSis/repo/Project/new/clipSocket
