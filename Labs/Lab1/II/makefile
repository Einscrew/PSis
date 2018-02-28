FLAGS=-g -Wall -std=c99

II: pointers.o
	gcc -o $@ $^ $(FLAGS)

pointers.o: pointers.c
	gcc -c $^ $(FLAGS)

clean:
	rm -f II *.o