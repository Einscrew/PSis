FLAGS=-ldl -fPIC -Wall

III: prog2.o
	gcc -o $@ $^ $(FLAGS)

prog2.o: prog2.c test.h
	gcc -c $^ $(FLAGS)

test1.so: test1.c test.h
	gcc $^ -o test1.so -shared $(FLAGS)

test2.so: test2.c test.h
	gcc $^ -o test2.so -shared $(FLAGS)

clean:
	rm -f *.*o III