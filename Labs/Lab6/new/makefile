CC=gcc
FLAGS=-Wall -g

all: clipboard app 
	
clipboard: clipboard.c library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

app: app_teste.c library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

library.o: library.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o app clipboard 