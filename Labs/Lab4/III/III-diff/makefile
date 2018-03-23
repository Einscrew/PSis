FLAGS=-g -Wall
EXTRA=
III: III.o
	gcc -o $@ $^ $(FLAGS) $(EXTRA)

III.o: III.c
	gcc -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o III
