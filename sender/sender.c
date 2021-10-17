#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define DYNAMIC_LEN 128
#define MSG_MAX_LEN 1024

static pthread_mutex_t dynamicMessageMutex = PTHREAD_MUTEX_INITIALIZER;

static int remotePort;
static int socketDescriptor;
static pthread_t threadPID;
static char* s_rxMessage;
static char* dynamicMessage;


void* sendThread(void* msgArg) {
    // Construct remote socket address
    struct sockaddr_in sinRemote;
    memset(&sinRemote, 0, sizeof(sinRemote));
    sinRemote.sin_family = AF_INET;                   // connection may be from network
    sinRemote.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long // local IP address
    sinRemote.sin_port = htons(remotePort);           // Host to Network short

    // TO DO: Read the message in the Input Module and add it to the list SenderList
    // Sender module will then pull the message from the list and send it

    printf("Enter a message: "); // to be removed
    char x;
    scanf("%c", &x);

    // Send the message to remote socket
    unsigned int sin_len = sizeof(sinRemote);
    char* messageTx = &x;
    sendto(socketDescriptor, 
        messageTx, MSG_MAX_LEN, 0, 
        (struct sockaddr *) &sinRemote, sin_len);
    return NULL;
}

void Sender_init(char* rxMessage, int port, int socket) {
    remotePort = port;
    socketDescriptor = socket;
    pthread_create(
        &threadPID,                  // PID (passed by pointer)
        NULL,                        // Attributed (not used usually)
        sendThread,                 // Function pointer
        NULL                        // Arguments (if unused un Function, then pass NULL)
    );
}

void Sender_shutdown(void) {
    // TO DO: cancel thread when user inputs ! char

    // Cancel the thread to finish
    // pthread_cancel(threadPID);

    // waits for thread to finish
    pthread_join(threadPID, NULL);
}