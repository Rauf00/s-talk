#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "keyboard.h"
#include "../sender/sender.h"
#include "../list/listmanager.h"

#define MSG_MAX_LEN 512

static pthread_t pthreadKeyboard;
static List* senderList;
static pthread_mutex_t keyboardMutex;
static char* message;
static pthread_cond_t buffAvail = PTHREAD_COND_INITIALIZER;

/* 
    Listens for the user input from the terminal
    and puts a message into senderList
*/
void* keyboardThread(void* empty) {
    while(1) {
        // Get message from the keyboard
        message = (char*) malloc(MSG_MAX_LEN);
        if(message == NULL) {
            puts("Keyboard: Message not malloc");
        }
        fgets(message, MSG_MAX_LEN, stdin);
        // Access to senderList (critical section), so lock the mutex
        pthread_mutex_lock(&keyboardMutex);
        {   
            // If the list is full, then block the process 
            // until consumer (Sender thread) signals buffAvail and frees a node
            if (List_count(senderList) == MSG_MAX_LEN) {
                pthread_cond_wait(&buffAvail,&keyboardMutex);
            }
            // If the list is not full, prepend a new item to the list
            // and wake up the consumer (Sender thread)
            if(List_prepend(senderList, message) == -1) {
                puts("Keyboard: Failed to prepend message");
            }
            Sender_itemAvailSignal();
        }
        pthread_mutex_unlock(&keyboardMutex);

        // Finish the program if the user inoput is "!" and clean up the memory
        if(strcmp(message,"!\n") == 0) {
            pthread_cond_destroy(&buffAvail);
            break;
        }
    }
    return NULL;
}

void Keyboard_buffAvailSignal() {
    pthread_mutex_lock(&keyboardMutex);
    { 
        pthread_cond_signal(&buffAvail);
    }
    pthread_mutex_unlock(&keyboardMutex);
}

void Keyboard_init(pthread_mutex_t mutex) {
    keyboardMutex = mutex;
    senderList = ListManager_getsenderList();
    pthread_create(
        &pthreadKeyboard,
        NULL,
        keyboardThread,
        NULL
    );
}

void Keyboard_join(void) {
    pthread_join(pthreadKeyboard,NULL);
}

void Keyboard_cancel(void) {
    free(message);
    pthread_cancel(pthreadKeyboard);
    pthread_cond_destroy(&buffAvail);
}