all: build

build:
	gcc socket/socket.c receiver/receiver.c sender/sender.c list/list.c keyboard/keyboard.c list/listmanager.c screen/screen.c main.c -lpthread -o main

run: build
	./main

valgrind: build
	valgrind --leak-check=full ./main

clean:
	rm -f main