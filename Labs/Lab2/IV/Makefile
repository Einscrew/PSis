FLAGS=-g -O0

char-conv-uninit: char-conv-uninit.o
	gcc -o $@ $^ $(FLAGS)

char-conv-uninit.o: char-conv-uninit.c
	gcc -c $^ $(FLAGS)

clean:
	rm -f *.o char-conv-uninit core
