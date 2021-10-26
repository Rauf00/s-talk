#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "keyboard.h"
#include "../sender/sender.h"
#include "../list/listmanager.h"

#define MSG_MAX_LEN 1024

static pthread_t pthreadKeyboard;
static List* senderList;
static pthread_mutex_t keyboardMutex;
static pthread_cond_t buffAvail = PTHREAD_COND_INITIALIZER;
static char* message;

//SENDS MESSAGE TO SENDER
//RECEIVES INPUT FROM TERMINAL
void* keyboardThread(void* empty) {
    while(1) {
        // Get message from the keyboard
        message = (char*) malloc(MSG_MAX_LEN);
        fgets(message, MSG_MAX_LEN, stdin);
        if(message == NULL) {
            puts("Keyboard: Message not malloc");
        }
        pthread_mutex_lock(&keyboardMutex);
        {   
            // If the list is full, then block the process 
            // until consumer signals buffAvail and free a node
            if (List_count(senderList) == MSG_MAX_LEN) {
                pthread_cond_wait(&buffAvail,&keyboardMutex);
            }
            // if the list is not full, prepend a new item to the list
            // and wake up the consumer
            if(List_prepend(senderList, message) == -1) {
                puts("Keyboard: Failed to prepend message");
            }
            Sender_itemAvailSignal();
        }
        pthread_mutex_unlock(&keyboardMutex);

        if(strcmp(message,"!\n") == 0) {
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
    List_free(senderList, NULL);
    pthread_cancel(pthreadKeyboard);
}