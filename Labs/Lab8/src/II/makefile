CC=gcc
FLAGS=-Wall -g -lpthread
ALL=clipboard app remoteBackup app2

all: $(ALL)

remoteBackup: remoteBackup.c library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

clipboard: clipboard.c library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

app: app_teste.c library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

app2: test.c library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

library.o: library.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o $(ALL)