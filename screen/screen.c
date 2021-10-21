#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "screen.h"
#include "../list/list.h"
#include "../receiver/receiver.h"
#include "../list/listmanager.h"

#define MSG_MAX_LEN 1024
// RECEIVES MESSAGE FROM RECEIVER AND DISPLAYS
static pthread_t thread;

static List* receiverList = NULL;
static char* message = NULL;

void* screenThread(void* empty) {
    while(1) {
        while(List_count(receiverList) > 0)  {
            message = List_trim(receiverList);
            if(strcmp(message,"!\n") == 0) {
                puts("PROGRAM SHUTDOWN");
                exit(1);
            }

            fputs("Receiver: ", stdout);
            fputs(message, stdout);

            free(message);
            message = NULL; 

        }
    }
    return NULL;

}

void Screen_init() {
    receiverList = ListManager_getreceiverList();
    pthread_create(
        &thread,
        NULL,
        screenThread,
        NULL
    );
}

void Screen_shutdown(void) {
    //TODO: cancel threads
    pthread_cancel(thread);
    if(message != NULL) {
        free(message);
        message = NULL;
    }
    pthread_join(thread,NULL);
}