CC=gcc
FLAGS=-Wall -g -lpthread
ALL=clipboard app remoteBackup

all: $(ALL)

# Runnable

remoteBackup: remoteBackup.c connection.o library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

clipboard: clipboard.c connection.c library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)
		
app: app_teste.c connection.c library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

# Libraries
connection: connection.c library.o
	$(CC) -c $^ $(FLAGS) $(EXTRA)

library.o: library.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o $(ALL)