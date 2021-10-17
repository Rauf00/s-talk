#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "receiver.h"

#define DYNAMIC_LEN 128
#define MSG_MAX_LEN 1024

static pthread_mutex_t dynamicMessageMutex = PTHREAD_MUTEX_INITIALIZER;

static int localPort;
static int socketDescriptor;
static pthread_t threadPID;
static char* s_rxMessage;
static char* dynamicMessage;


void* receiveThread(void* msgArg) { // this arg has to be always passed even if unused
    // printf("Receiver is listening...\n");
    while (1) {
        // get the data (blocking)
        // Will change sin (the address) to be the address of the client 
        // Note: sin passes information in and otu of call!
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        char messageRx[MSG_MAX_LEN];
        int recieveMessegeSize = recvfrom(socketDescriptor, 
            messageRx, MSG_MAX_LEN, 0, 
            (struct sockaddr *) &sinRemote, &sin_len);
        messageRx[recieveMessegeSize] = '\0';
        printf("Message: %s\n", messageRx);
    }
    
    return NULL;
}

void Receiver_init(char* rxMessage, int port, int socket) {
    localPort = port;
    socketDescriptor = socket;
    // s_rxMessage = localPort;
    pthread_create(
        &threadPID,                     // PID (passed by pointer)
        NULL,                           // Attributed (not used usually)
        receiveThread,                  // Function pointer
        "This is a new arg\n"           // Arguments (if unused un Function, then pass NULL)
    );
}

void Receiver_shutdown(void) {
    // Cancel the thread to finish
    // pthread_cancel(threadPID);

    // waits for thread to finish
    pthread_join(threadPID, NULL);
}