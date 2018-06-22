FLAGS=-g -Wall

II: II.o
	gcc -o $@ $^ $(FLAGS)

II.o: II.c
	gcc -c $^ $(FLAGS)

clean:
	rm -f *.*o II
