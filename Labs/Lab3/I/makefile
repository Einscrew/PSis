FLAGS=-g -Wall

I: I.o
	gcc -o $@ $^ $(FLAGS)

I.o: I.c
	gcc -c $^ $(FLAGS)

clean:
	rm -f *.*o I
