#include <stdio.h>
#include <stdlib.h>
#include "receiver.h"
#include "sender.h"
#include "socket.h"

int main(int argc, char** args) {
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
    // Startup my modules
    Receiver_init(NULL, localPort, socketDescriptor);

    // printf("Enter a message: \n");
    // char x;
    // scanf("%c", &x);
    Sender_init(NULL, remotePort, socketDescriptor);

    // // Wait for the user input
    

    // Shutdown my modules
    Receiver_shutdown();
    Sender_shutdown();
    printf("done\n");

    return 0;
}
