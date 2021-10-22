#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "screen.h"
#include "../receiver/receiver.h"
#include "../list/listmanager.h"

#define MSG_MAX_LEN 1024

static pthread_t pthreadScreen;
static pthread_mutex_t displayMutex;
static List* receiverList;
static char* message;
static pthread_cond_t itemAvail = PTHREAD_COND_INITIALIZER;

void* screenThread(void* empty) {
    while(1) {
        // if the list is empty, there is nothing to consume, so block
        pthread_mutex_lock(&displayMutex);
            { 
                if (List_count(receiverList) == 0)  {
                    pthread_cond_wait(&itemAvail,&displayMutex);
                }
                // if there is an item in the list, trim it
                message = List_trim(receiverList);
                // Signal producer that a new buffer is available
                Receiver_buffAvailSignal();
            }
        pthread_mutex_unlock(&displayMutex);

        // Consume
        if(strcmp(message,"!\n") == 0) {
            puts("PROGRAM SHUTDOWN");
            exit(1);
        }
        fputs("Incoming message: ", stdout);
        fputs(message, stdout);
        message = NULL; 
    }
    return NULL;
}

void Screen_itemAvailSignal() {
    pthread_mutex_lock(&displayMutex);
    { 
        pthread_cond_signal(&itemAvail);
    }
    pthread_mutex_unlock(&displayMutex);
}

void Screen_init(pthread_mutex_t mutex) {
    displayMutex = mutex;
    receiverList = ListManager_getreceiverList();
    pthread_create(
        &pthreadScreen,
        NULL,
        screenThread,
        NULL
    );
}

void Screen_shutdown(void) {
    //TODO: cancel threads
    pthread_cancel(pthreadScreen);
    if(message != NULL) {
        free(message);
        message = NULL;
    }
    pthread_join(pthreadScreen,NULL);
}