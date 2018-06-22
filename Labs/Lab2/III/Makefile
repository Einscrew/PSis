FLAGS=-g -O0

char-conv-prob: char-conv-prob.o
	gcc -o $@ $^ $(FLAGS)

char-conv-prob.o: char-conv-prob.c
	gcc -c $^ $(FLAGS)

clean:
	rm -f *.o char-conv-prob core
