#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

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
static char* message;
static pthread_cond_t buffAvail = PTHREAD_COND_INITIALIZER;

/* 
    Listens for the messages from remote sender
    and puts a message into receiverList
*/
void* receiveThread(void* msgArg) {
    while (1) {
        // Listen for a receiveing message
        struct sockaddr_in sinRemote;
        memset(&sinRemote, 0, sizeof(sinRemote));
        unsigned int sin_len = sizeof(sinRemote);
        message = (char*) malloc(MSG_MAX_LEN);
        if(recvfrom(socketDescriptor, message, MSG_MAX_LEN, 0,  (struct sockaddr *) &sinRemote, &sin_len) == -1) {
            puts("Receiver: Failed to recvfrom");
            return NULL;
        }
        
        // Access to recevierList (critical section), so lock the mutex
        pthread_mutex_lock(&displayMutex);
        {   
            // If the list is full, then block the process 
            // until consumer (Screen thread) signals buffAvail and frees a node
            if (List_count(receiverList) == MSG_MAX_LEN) {
                pthread_cond_wait(&buffAvail,&displayMutex);
            }
            // If the list is not full, prepend a new item to the receiverList
            // and wake the consumer (Screen thread) up
            if(List_prepend(receiverList, message) == -1) {
                puts("Keyboard: Failed to prepend message");
            }
            Screen_itemAvailSignal();
        }
        pthread_mutex_unlock(&displayMutex);

        if(message == NULL) {
            puts("Receiver: Message is NULL");
        }
        
        // Finish the program if receiving message is "!" and clean up the memory
        if(strcmp(message,"!\n") == 0) {
            pthread_cond_destroy(&buffAvail);
            // Close socket after ! is received and added to the reciverList
            close(socketDescriptor);
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
    // Clean up the memory
    free(message);
    List_free(receiverList, ListManager_freeNode);
    close(socketDescriptor);
    pthread_cancel(pthreadReceiver);
    pthread_mutex_destroy(&displayMutex);
    pthread_cond_destroy(&buffAvail);
}