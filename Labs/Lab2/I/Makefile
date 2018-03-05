FLAGS=-g -O0

inf_loop: infinite-loop.o
	gcc -o $@ $^ $(FLAGS)

infinite-loop.o: infinite-loop.c
	gcc -c $^ $(FLAGS)

clean:
	rm -f *.o inf_loop
