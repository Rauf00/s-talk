#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "list.h"

//Handles Receiver and Sender message Lists

static List* receiverList;
static List* senderList;

int ListManager_create() {
    receiverList = List_create();
    if(receiverList == NULL) return 1;
    senderList = List_create();
    if(senderList == NULL) return 1;

    return 0;
}
List* ListManager_getsenderList() {
    return senderList;
}
List* ListManager_getreceiverList() {
    return receiverList;
}

void ListManager_freeNode(void* node) {
    free(node);
}