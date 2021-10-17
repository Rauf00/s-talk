#include <stdio.h>
#include <stdlib.h>
#include "receiver/receiver.h"
#include "sender/sender.h"
#include "socket/socket.h"

int main(int argc, char** args) {
    // Read arguments
    if (argc != 4) {
        printf("Invalid num of args\n");
        return 0;
    }
    int localPort = atoi(args[1]);
    char* remoteName = args[2];
    int remotePort = atoi(args[3]);
    printf("Local port: %d, Remote name: %s, Remote port: %d\n", localPort, remoteName, remotePort);

    // Create socket
    int socketDescriptor = Socket_init(localPort);

    // Startup modules (we'll have 4 threads)
    Receiver_init(NULL, localPort, socketDescriptor); // Gets messsage from other process and puts it to the list 
    Sender_init(NULL, remotePort, socketDescriptor);  // Gets message from the list and sends it to the other process
    // Input_init(); // Gets input from user and puts it to the list
    // Ouput_init(); // Gets output from the list and displays it

    // Shutdown modules
    Receiver_shutdown();
    Sender_shutdown();

    printf("**** Program end ****\n");
    return 0;
}
