FLAGS=-g -O0

char-conv: char-conv.o
	gcc -o $@ $^ $(FLAGS)

char-conv.o: char-conv.c
	gcc -c $^ $(FLAGS)
