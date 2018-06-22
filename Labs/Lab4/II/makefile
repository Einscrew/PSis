FLAGS=-g -Wall
EXTRA=
II: II.o
	gcc -o $@ $^ $(FLAGS) $(EXTRA)

II.o: II.c
	gcc -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o II
