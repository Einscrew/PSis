CC=gcc
FLAGS=-Wall

all: clipboard app

clipboard: clipboard.c library.o
	$(CC) -o $@ $^ $(FLAGS) $(EXTRA)

app: app_test.c library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)


library.o: library.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)