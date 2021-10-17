all: build

build:
	gcc socket.c receiver.c sender.c main.c -lpthread -o main

run: build
	./main

valgrind: build
	valgrind --leak-check=full ./main

clean:
	rm -f main