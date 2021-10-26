#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "receiver.h"
#include "../screen/screen.h"
#include "../list/listmanager.h"
#include "../cancelThreads/cancelThreads.h"

#define MSG_MAX_LEN 512

static int localPort;
static int socketDescriptor;
static pthread_t pthreadReceiver;
static pthread_mutex_t displayMutex;
static List* receiverList;
static pthread_cond_t buffAvail = PTHREAD_COND_INITIALIZER;
static char* message;

void* receiveThread(void* msgArg) {
    while (1) {
        // Listen for a receiveing message
        struct sockaddr_in sinRemote;
        memset(&sinRemote, 0, sizeof(sinRemote));
        unsigned int sin_len = sizeof(sinRemote);
        message = (char*) malloc(MSG_MAX_LEN);
        
        if(recvfrom(socketDescriptor,message, MSG_MAX_LEN, 0,  (struct sockaddr *) &sinRemote, &sin_len) == -1) {
            puts("Receiver: Failed to recvfrom");
            return NULL;
        }
        
        pthread_mutex_lock(&displayMutex);
        {   
            // If the list is full, then block the process 
            // until consumer (Screen thread) signals buffAvail and free a node
            if (List_count(receiverList) == MSG_MAX_LEN) {
                pthread_cond_wait(&buffAvail,&displayMutex);
            }
            // if the list is not full, prepend a new item to the list
            // and wake the consumer (Screen thread) up
            List_prepend(receiverList, message);
            Screen_itemAvailSignal();
        }
        pthread_mutex_unlock(&displayMutex);

        if(message == NULL) {
            puts("Receiver: Message is NULL");
        }
        
        // Finish the program if receiving message is "!"
        if(strcmp(message,"!\n") == 0) {
            break;
        }
    }
    return NULL;
}

void Receiver_buffAvailSignal() {
    pthread_mutex_lock(&displayMutex);
    { 
        pthread_cond_signal(&buffAvail);
    }
    pthread_mutex_unlock(&displayMutex);
}

void Receiver_init(int port, int socket, pthread_mutex_t mutex) {
    localPort = port;
    socketDescriptor = socket;
    displayMutex = mutex;
    receiverList = ListManager_getreceiverList();
    pthread_create(
        &pthreadReceiver,
        NULL,
        receiveThread,
        NULL
    );
}

void Receiver_join(void) {
    pthread_join(pthreadReceiver, NULL);
}

void Receiver_cancel(void) {
    free(message);
    List_free(receiverList, NULL); // ask TA
    pthread_cancel(pthreadReceiver);
}