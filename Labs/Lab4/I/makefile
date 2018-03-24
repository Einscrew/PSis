FLAGS=-g -Wall
EXTRA=

all: I-pipes I-single-pipe
	
I-pipes: I-pipes.o
	gcc -o $@ $^ $(FLAGS) $(EXTRA)

I-pipes.o: I-pipes.c
	gcc -c $^ $(FLAGS) $(EXTRA)

I-single-pipe: I-single-pipe.o
	gcc -o $@ $^ $(FLAGS) $(EXTRA)

I-single-pipe.o: I-single-pipe.c
	gcc -c $^ $(FLAGS) $(EXTRA)
clean:
	rm -f *.*o I-pipes I-single-pipe
