CC=gcc
FLAGS=-Wall -g -lpthread

all: clipboard app


clipboard: clipboard.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

app: app.o library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)	
		
# Libraries

app.o: app.c library.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

library.o: library.c library.h clip.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)


clipboard.o: clipboard.c clip.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)


clean:
	rm -f *.*o *.gch clipboard app


#           clip
#          /    \
#   wait          cl.wait()
#   clipboard
#   
#   
#   
#   
#   
#   
#   