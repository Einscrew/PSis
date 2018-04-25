CC=gcc
FLAGS=-Wall -g -lpthread
ALL=clipboard app remoteBackup

all: $(ALL)

remoteBackup: remoteBackup.c library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

clipboard: clipboard.c library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

app: app_teste.c library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

library.o: library.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o $(ALL)