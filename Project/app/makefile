CC=gcc
FLAGS=-lpthread
EXTRA=-Wall -g
APP=app app_test app_threads
ALL=$(APP)
LIB=../lib

all: $(ALL)

#############################################################
#						 Objects                            #
#############################################################
app_threads: app_threads.c $(LIB)/connection.o $(LIB)/library.o $(LIB)/list.o $(LIB)/utils.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

app_test: app_teste.c $(LIB)/connection.o $(LIB)/library.o $(LIB)/list.o $(LIB)/utils.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

app: app.c $(LIB)/connection.o $(LIB)/library.o $(LIB)/list.o $(LIB)/utils.o
	$(CC) $^ -o $@ $(FLAGS) $(EXTRA)

clean:
	rm -f $(ALL)
