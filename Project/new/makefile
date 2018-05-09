CC=gcc
FLAGS=-Wall -g -lpthread
ALL=clipboard app 
#remoteBackup

all: $(ALL)

# Runnable

#remoteBackup: remoteBackup.c connection.o library.o
#	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

clipboard: clipboard.c connection.o library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)
		
app: app_teste.c connection.c library.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

# Libraries

connection.o: connection.c connection.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

library.o: library.c clipboard.h connection.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o *.gch $(ALL)