#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "receiver/receiver.h"
#include "sender/sender.h"
#include "socket/socket.h"
#include "keyboard/keyboard.h"
#include "list/listmanager.h"
#include "screen/screen.h"

int main(int argc, char** args) {
    // Read arguments
    if (argc != 4) {
        printf("Invalid num of args\n");
        return 0;
    }
    int localPort = atoi(args[1]);
    char* remoteName = args[2];
    char* remotePort = args[3];
    printf("Local port: %d, Remote name: %s, Remote port: %s\n", localPort, remoteName, remotePort);

    //create sender/receiver lists
    ListManager_create();

    // Create socket
    int socketDescriptor = Socket_init(localPort);

    // displayMutex is shared between Receiver and Screen
    pthread_mutex_t displayMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t keyboardMutex = PTHREAD_MUTEX_INITIALIZER;

    // Startup module
    Keyboard_init(keyboardMutex); // Gets input from user and puts it to the list
    Sender_init(remoteName,remotePort, socketDescriptor, keyboardMutex);  // Gets message from the list and sends it to the other process
    Receiver_init(localPort, socketDescriptor, displayMutex); // Gets messsage from other process and puts it to the list 
    Screen_init(displayMutex);

    // join modules
    Screen_join();
    Keyboard_join();
    Sender_join();
    Receiver_join();
    
    printf("**** Program end ****\n");
    return 0;
}
