#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "../sender/sender.h"
#include "../keyboard/keyboard.h"
#include "../list/listmanager.h"

#define MSG_MAX_LEN 1024

static int remotePort;
static int socketDescriptor;
static pthread_t thread;
static List* senderList;
static char* message = NULL;
pthread_mutex_t keyboardMutex;
static pthread_cond_t itemAvail = PTHREAD_COND_INITIALIZER;


void* sendThread(void* msgArg) {
    // Construct remote socket address
    struct sockaddr_in sinRemote;
    memset(&sinRemote, 0, sizeof(sinRemote));
    sinRemote.sin_family = AF_INET;                   // connection may be from network
    sinRemote.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long // local IP address
    sinRemote.sin_port = htons(remotePort);           // Host to Network short

    while(1) {
        pthread_mutex_lock(&keyboardMutex);
        { 
            if (List_count(senderList) == 0)  {
                pthread_cond_wait(&itemAvail,&keyboardMutex);
            }
            // if there is an item in the list, trim it
            message = List_trim(senderList);
            // Signal producer that a new buffer is available
            Keyboard_buffAvailSignal();
        }
        pthread_mutex_unlock(&keyboardMutex);

        // Send the message to remote socket
        unsigned int sin_len = sizeof(sinRemote);
        sendto(socketDescriptor, 
            message, strlen(message), 0, 
            (struct sockaddr *) &sinRemote, sin_len);

        if(strcmp(message,"!\n") == 0) {
            puts("PROGRAM SHUTDOWN");
            exit(1);
        }
        free(message);
        message = NULL;
    }
    return NULL;
}

void Sender_itemAvailSignal() {
    pthread_mutex_lock(&keyboardMutex);
    { 
        pthread_cond_signal(&itemAvail);
    }
    pthread_mutex_unlock(&keyboardMutex);
}

void Sender_init(int port, int socket, pthread_mutex_t mutex) {
    remotePort = port;
    socketDescriptor = socket;
    keyboardMutex = mutex;
    senderList = ListManager_getsenderList();
    pthread_create(
        &thread,
        NULL,
        sendThread,
        NULL
    );
}

void Sender_shutdown(void) {
    pthread_cancel(thread);
    pthread_join(thread, NULL);
}