FLAGS=-g -Wall

III: III.o
	gcc -o $@ $^ $(FLAGS)

III.o: III.c
	gcc -c $^ $(FLAGS)

clean:
	rm -f *.*o
