all:
	gcc -g -Wall socket/socket.c receiver/receiver.c sender/sender.c list/list.c cancelThreads/cancelThreads.c keyboard/keyboard.c list/listmanager.c screen/screen.c main.c -lpthread -o s-talk

clean:
	rm -f s-talk