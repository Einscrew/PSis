CC=gcc
FLAGS=-lpthread
EXTRA=-Wall -g
APP=app app_test app_threads
ALL=$(APP)

LIBS=connection.o library.o utils.o

all: $(ALL)

#############################################################
#						 Objects                            #
#############################################################
app_threads: app_threads.c $(LIBS)
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

app_test: app_teste.c $(LIBS)
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

app: app.c $(LIBS)
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

clean:
	rm -f $(ALL)
