FLAGS=-g -Wall
EXTRA=

all: VI-producer VI-consumer

VI-producer: VI-producer.o
	gcc -o $@ $^ $(FLAGS) $(EXTRA)

VI-consumer: VI-consumer.o
	gcc -o $@ $^ $(FLAGS) $(EXTRA)

VI-producer.o: VI-producer.c
	gcc -c $^ $(FLAGS) $(EXTRA)

VI-consumer.o: VI-consumer.c
	gcc -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o VI-producer VI-consumer
