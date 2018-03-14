FLAGS=-g -Wall

IV: IV.o
	gcc -o $@ $^ $(FLAGS)

IV.o: IV.c
	gcc -c $^ $(FLAGS)

clean:
	rm -f *.*o IV
