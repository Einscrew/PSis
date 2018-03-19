FLAGS= -g -Wall

VI: lab3-mult-debug.o
	gcc -o $@ $^ $(FLAGS)

lab3-mult-debug.o: lab3-mult-debug.c
	gcc -c $^ $(FLAGS)

clean:
	rm -f *.*o VI
