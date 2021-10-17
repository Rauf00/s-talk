#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "socket.h"

int Socket_init(int localPort) { // this arg has to be always passed even if unused
    // Address
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;                   // connection may be from network
    sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long // local IP address
    sin.sin_port = htons(localPort);                 // Host to Network short

    // Create the socket for UDP
    int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // Bind the socket to the port (PORT) that we specify
    bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
    
    return socketDescriptor;
}