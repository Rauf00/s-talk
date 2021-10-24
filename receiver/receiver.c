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

#define MSG_MAX_LEN 1024

static int localPort;
static int socketDescriptor;
static pthread_t pthreadReceiver;
static pthread_mutex_t displayMutex;
static List* receiverList;
static pthread_cond_t buffAvail = PTHREAD_COND_INITIALIZER;

void* receiveThread(void* msgArg) {
    while (1) {
        // Listen for a receiveing message
        struct sockaddr_in sinRemote;
        memset(&sinRemote, 0, sizeof(sinRemote));
        unsigned int sin_len = sizeof(sinRemote);
        char message[MSG_MAX_LEN] = {};
        recvfrom(socketDescriptor, 
            message, MSG_MAX_LEN, 0, 
            (struct sockaddr *) &sinRemote, &sin_len);

        // Remove next line char (might find a better way)
        char msgWithoutNextLine[MSG_MAX_LEN];
        for (int i = 0; i < MSG_MAX_LEN - 1; i++) {
            msgWithoutNextLine[i] = message[i];
        }

        // Finish the program if receiving message is "!"
        if(strcmp(msgWithoutNextLine,"!\n") == 0) {
            List_free(receiverList, free); // seems that it doesnt free the list propely. Confirm with TA
            CancelThreads_cancelAllThreadsInReceiver();
        }
        
        pthread_mutex_lock(&displayMutex);
        {   
            // If the list is full, then block the process 
            // until consumer signals buffAvail and free a node
            if (List_count(receiverList) == MSG_MAX_LEN) {
                pthread_cond_wait(&buffAvail,&displayMutex);
            }
            // if the list is not full, prepend a new item to the list
            // and wake up the consumer
            List_prepend(receiverList, msgWithoutNextLine);
            Screen_itemAvailSignal();
        }
        pthread_mutex_unlock(&displayMutex);
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
    receiverList = ListManager_getreceiverList();
    localPort = port;
    socketDescriptor = socket;
    displayMutex = mutex;
    pthread_create(
        &pthreadReceiver,
        NULL,
        receiveThread,
        NULL
    );
}

void Receiver_join(void) {
    // printf("receveir join");
    pthread_join(pthreadReceiver, NULL);
}

void Receiver_cancel(void) {
    // printf("receiver cancel\n");
    pthread_cancel(pthreadReceiver);
}