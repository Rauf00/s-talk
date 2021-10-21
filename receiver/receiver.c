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
#include "../screen/screen.h"

#define MSG_MAX_LEN 1024

static pthread_mutex_t dynamicMessageMutex = PTHREAD_MUTEX_INITIALIZER;

static int localPort;
static int socketDescriptor;
static pthread_t threadPID;
static char* s_rxMessage;
static char* dynamicMessage;

static pthread_mutex_t display_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t bufferAvail = PTHREAD_COND_INITIALIZER;
static pthread_cond_t itemAvail = PTHREAD_COND_INITIALIZER;

static List* receiverList = NULL;

void* receiveThread(void* msgArg) { // this arg has to be always passed even if unused
    while (1) {
        // get the data (blocking)
        // Will change sin (the address) to be the address of the client 
        // Note: sin passes information in and otu of call!
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        char message[MSG_MAX_LEN] = {};

        if(List_count(receiverList) == 100) {
            pthread_mutex_lock(&display_mutex);
            {
                // wait will gave up the mutex while process is waiting
                pthread_cond_wait(&bufferAvail, &display_mutex);
            }
            pthread_mutex_unlock(&display_mutex);
        }
        recvfrom(socketDescriptor, 
        message, MSG_MAX_LEN, 0, 
        (struct sockaddr *) &sinRemote, &sin_len);

        pthread_mutex_lock(&display_mutex);
        {
            List_prepend(receiverList,message);
            printf("%s", message);
            pthread_cond_signal(&itemAvail);
        }
        pthread_mutex_unlock(&display_mutex);
        printf("mutex unlocked receiver");
           
    }
    
    return NULL;
}

void Receiver_itemAvailWait() {

    pthread_mutex_lock(&display_mutex);
    { 
        pthread_cond_wait(&itemAvail,&display_mutex);
    }
    pthread_mutex_unlock(&display_mutex);
}

void Receiver_bufferAvailSignal() {

    pthread_mutex_lock(&display_mutex);
    { 
        pthread_cond_signal(&bufferAvail);
    }
    pthread_mutex_unlock(&display_mutex);
}


void Receiver_init(char* rxMessage, int port, int socket) {
    printf("receiver starting\n");
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