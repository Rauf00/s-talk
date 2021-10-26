#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>

int Socket_init(int localPort) {
    // Address
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(localPort);

    // Create the socket for UDP
    int socketDescriptor;
    if((socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        puts("Receiver: Failed to connect to socket");
        return 1;
    }

    // Bind the socket to the port (PORT) that we specify
    if(bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin)) == -1) {
        puts("Receiver: Failed to bind");
        return 1;
    }
    
    return socketDescriptor;
}