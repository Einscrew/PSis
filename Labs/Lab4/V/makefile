FLAGS=-g -Wall
EXTRA=

all: V-producer V-consumer

V-producer: V-producer.o
	gcc -o $@ $^ $(FLAGS) $(EXTRA)

V-consumer: V-consumer.o
	gcc -o $@ $^ $(FLAGS) $(EXTRA)

V-producer.o: V-producer.c
	gcc -c $^ $(FLAGS) $(EXTRA)

V-consumer.o: V-consumer.c
	gcc -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o V-producer V-consumer
