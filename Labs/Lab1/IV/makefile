
III: prog2.o test1.so test2.so
	gcc -o $@ $^ -ldl -fPIC

prog2.o: prog2.c test1.so test2.so
	gcc -c $^ -ldl -fPIC

test1.so: test1.c test.h
	gcc $^ -o test1.so -ldl -shared -fPIC

test2.so: test2.c test.h
	gcc $^ -o test2.so -ldl -shared -fPIC


clean:
	rm -f *.*o III