FLAGS=-g -Wall

V: V.o
	gcc -o $@ $^ $(FLAGS)

V.o: V.c
	gcc -c $^ $(FLAGS)

clean:
	rm -f *.*o V
