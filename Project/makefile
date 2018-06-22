ALL=lib clipboard app

.PHONY:$(ALL)

all: $(ALL)

lib:
	cd lib/ && make

clipboard:
	cd clipboard/ && make

app: 
	cd app/ && make

clean:
	make clean -C lib/ && make clean -C clipboard/ && make clean -C app/
