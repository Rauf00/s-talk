#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "screen.h"
#include "../receiver/receiver.h"
#include "../list/listmanager.h"

#define MSG_MAX_LEN 1024

static pthread_t pthreadScreen;
static List* receiverList = NULL;
static char* message = NULL;
static pthread_cond_t itemAvail = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t display_mutex;

void* screenThread(void* empty) {
    while(1) {
        // if the list is empty, there is nothing to consume, so block
        if (List_count(receiverList) == 0)  {
            pthread_mutex_lock(&display_mutex);
            { 
                pthread_cond_wait(&itemAvail,&display_mutex);
            }
            pthread_mutex_unlock(&display_mutex);
        }

        // if there is an item in the list, trim it
        // and signal producer that a new buffer is available
        pthread_mutex_lock(&display_mutex);
        {
            message = List_trim(receiverList);
        }
        pthread_mutex_unlock(&display_mutex);
        Receiver_buffAvailSignal();

        // Consume
        if(strcmp(message,"!\n") == 0) {
            puts("PROGRAM SHUTDOWN");
            exit(1);
        }
        fputs("Receiver: ", stdout);
        fputs(message, stdout);
        message = NULL; 
    }
    return NULL;
}

void Screen_itemAvailSignal() {
    pthread_mutex_lock(&display_mutex);
    { 
        pthread_cond_signal(&itemAvail);
    }
    pthread_mutex_unlock(&display_mutex);
}

void Screen_init(pthread_mutex_t mutex) {
    display_mutex = mutex;
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