#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "keyboard.h"
#include "../sender/sender.h"
#include "../list/listmanager.h"

#define MSG_MAX_LEN 1024

static pthread_t thread;
static List* senderList;
static pthread_mutex_t keyboardMutex;
static pthread_cond_t buffAvail = PTHREAD_COND_INITIALIZER;

//SENDS MESSAGE TO SENDER
//RECEIVES INPUT FROM TERMINAL
void* keyboardThread(void* empty) {
    char message[MSG_MAX_LEN] = {};
    while(1) {
        // Get message from the keyboard
        fgets(message, MSG_MAX_LEN, stdin);
        pthread_mutex_lock(&keyboardMutex);
        {   
            // If the list is full, then block the process 
            // until consumer signals buffAvail and free a node
            if (List_count(senderList) == MSG_MAX_LEN) {
                pthread_cond_wait(&buffAvail,&keyboardMutex);
            }
            // if the list is not full, prepend a new item to the list
            // and wake up the consumer
            List_prepend(senderList, message);
            Sender_itemAvailSignal();
        }
        pthread_mutex_unlock(&keyboardMutex);

        if(strcmp(message,"!\n") == 0) {
            puts("PROGRAM SHUTDOWN");
            exit(1);
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
        &thread,
        NULL,
        keyboardThread,
        NULL
    );
}

void Keyboard_shutdown(void) {
    /*
    pthread_cancel(thread);
    if(message != NULL) {
        free(message);
    }
    */
    pthread_join(thread,NULL);
}