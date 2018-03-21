FLAGS=-g -Wall
EXTRA=
I: I.o
	gcc -o $@ $^ $(FLAGS) $(EXTRA)

I.o: I.c
	gcc -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o I
