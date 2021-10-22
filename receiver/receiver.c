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

#define MSG_MAX_LEN 1024

static int localPort;
static int socketDescriptor;
static pthread_t pthreadReceiver;
static List* receiverList = NULL;
static pthread_cond_t buffAvail = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t display_mutex;

void* receiveThread(void* msgArg) { // this arg has to be always passed even if unused
    while (1) {
        // Listen for a receiveing message
        struct sockaddr_in sinRemote;
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

        // If the list is full, then block the process 
        // until consumer signals buffAvail and free a node
        if (List_count(receiverList) == MSG_MAX_LEN) {
            pthread_mutex_lock(&display_mutex);
            {
                pthread_cond_wait(&buffAvail,&display_mutex);
            }
            pthread_mutex_unlock(&display_mutex);
        }

        // if the list is not full, prepend a new item to the list
        // and wake up the consumer
        pthread_mutex_lock(&display_mutex);
        {
            List_prepend(receiverList, msgWithoutNextLine);
            Screen_itemAvailSignal();
        }
        pthread_mutex_unlock(&display_mutex);
    }
    return NULL;
}

void Receiver_buffAvailSignal() {
    pthread_mutex_lock(&display_mutex);
    { 
        pthread_cond_signal(&buffAvail);
    }
    pthread_mutex_unlock(&display_mutex);
}

void Receiver_init(char* rxMessage, int port, int socket, pthread_mutex_t mutex) {
    display_mutex = mutex;
    receiverList = ListManager_getreceiverList();
    localPort = port;
    socketDescriptor = socket;
    pthread_create(
        &pthreadReceiver,                     // PID (passed by pointer)
        NULL,                           // Attributed (not used usually)
        receiveThread,                  // Function pointer
        NULL           // Arguments (if unused un Function, then pass NULL)
    );
}

void Receiver_shutdown(void) {
    pthread_cancel(pthreadReceiver);
    pthread_join(pthreadReceiver, NULL);
    close(socketDescriptor);
}