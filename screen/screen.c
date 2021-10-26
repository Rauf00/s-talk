#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "screen.h"
#include "../receiver/receiver.h"
#include "../list/listmanager.h"
#include "../cancelThreads/cancelThreads.h"

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
        
        if(message == NULL) {
            puts("Screen: message is NULL");
        }

        // Consume
        if(strlen(message) > 1) {
            fputs("Incoming message: ", stdout);
            fputs(message, stdout);
        }

        // Finish the program if receiving message is "!"
        if(strcmp(message,"!\n") == 0) {
            free(message);
            List_free(receiverList, NULL); // seems that it doesnt free the list propely. Confirm with TA
            CancelThreads_keyboardAndSenderCancel();
            break;
        }
        free(message);
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

void Screen_join(void) {
    pthread_join(pthreadScreen, NULL);
}

void Screen_cancel(void) {
    pthread_cancel(pthreadScreen);
}