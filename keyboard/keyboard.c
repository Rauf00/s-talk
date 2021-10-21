#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "keyboard.h"
#include "../sender/sender.h"
#include "../list/list.h"
#include "../list/listmanager.h"

static pthread_t thread;

static List* senderList;
static char* message = NULL;

#define MSG_MAX_LEN 1024

//SENDS MESSAGE TO SENDER
//RECEIVES INPUT FROM TERMINAL
void* keyboardThread(void* empty) {
    while(1) {
        message = malloc(MSG_MAX_LEN);

        fgets(message, MSG_MAX_LEN, stdin);
        
        List_prepend(senderList,message);

        if(strcmp(message,"!\n") == 0) {
            puts("PROGRAM SHUTDOWN");
            exit(1);
        }
    }
    return NULL;
}

void Keyboard_init() {
    senderList = ListManager_getsenderList();
    pthread_create(
        &thread,                  // PID (passed by pointer)
        NULL,                        // Attributed (not used usually)
        keyboardThread,                 // Function pointer
        NULL                        // Arguments (if unused un Function, then pass NULL)
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