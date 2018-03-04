LIB=test2.o

III: prog1.o $(LIB)
	gcc -o $@ $^

prog1.o: prog1.c test.h
	gcc -c $^

test1.o: test1.c test.h
	gcc -c $^

test2.o: test2.c test.h
	gcc -c $^

clean:
	rm -f *.o III