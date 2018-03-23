FLAGS=-g -Wall
EXTRA=
V: V.o
	gcc -o $@ $^ $(FLAGS) $(EXTRA)

V.o: V.c
	gcc -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o V
