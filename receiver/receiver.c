#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "receiver.h"
#include "../list/list.h"
#include "../list/listmanager.h"

#define MSG_MAX_LEN 1024

static pthread_mutex_t dynamicMessageMutex = PTHREAD_MUTEX_INITIALIZER;

static int localPort;
static int socketDescriptor;
static pthread_t threadPID;
static char* s_rxMessage;
static char* dynamicMessage;

static List* receiverList = NULL;
static char* message = NULL;

void* receiveThread(void* msgArg) { // this arg has to be always passed even if unused
    while (1) {
        // get the data (blocking)
        // Will change sin (the address) to be the address of the client 
        // Note: sin passes information in and otu of call!
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        message = malloc(MSG_MAX_LEN);
        fflush(stdout);
        while(1) {
            int receiveMessageSize = recvfrom(socketDescriptor, 
            message, MSG_MAX_LEN, 0, 
            (struct sockaddr *) &sinRemote, &sin_len);
            List_prepend(receiverList,message);
        }
        // TO DO: put the message into the ReceiverList from which Output module will pull the message and display
        //printf("\nIncoming message: %p\n", List_curr(receiverList)); // to be removed
    }
    
    return NULL;
}

void Receiver_init(char* rxMessage, int port, int socket) {
    receiverList = ListManager_getreceiverList();
    localPort = port;
    socketDescriptor = socket;
    pthread_create(
        &threadPID,                     // PID (passed by pointer)
        NULL,                           // Attributed (not used usually)
        receiveThread,                  // Function pointer
        "This is a new arg\n"           // Arguments (if unused un Function, then pass NULL)
    );
}

void Receiver_shutdown(void) {
    // TO DO: cancel thread when user inputs ! char
    // Cancel the thread to finish
    pthread_cancel(threadPID);

    // waits for thread to finish
    pthread_join(threadPID, NULL);

    close(socketDescriptor);
}