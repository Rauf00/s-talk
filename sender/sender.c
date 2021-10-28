#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>

#include "../sender/sender.h"
#include "../keyboard/keyboard.h"
#include "../list/listmanager.h"
#include "../cancelThreads/cancelThreads.h"

#define MSG_MAX_LEN 512

static char* remotePort;
static int socketDescriptor;
static char* ipAddress;
int status;
static struct addrinfo *servinfo;
static pthread_t pthreadSender;
static List* senderList;
static char* message = NULL;
pthread_mutex_t keyboardMutex;
static pthread_cond_t itemAvail = PTHREAD_COND_INITIALIZER;

/*
    Gets the messages from the senderList
    and sends the messages to the remote receiver
*/
void* sendThread(void* msgArg) {
    // Construct remote socket address
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    status = getaddrinfo(ipAddress, remotePort, &hints, &servinfo);
    if(status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    if(socketDescriptor == -1) {
        puts("Sender: Failed to connect to socket");
    }

    while(1) {
        // Access to senderList (critical section), so lock the mutex
        pthread_mutex_lock(&keyboardMutex);
        { 
            if (List_count(senderList) == 0)  {
                pthread_cond_wait(&itemAvail,&keyboardMutex);
            }
            // If there is an item in the senderList, trim it
            message = (char*) List_trim(senderList);
            // Signal producer (Keyboard thread) that a new buffer is available
            Keyboard_buffAvailSignal();
        }
        pthread_mutex_unlock(&keyboardMutex);

        if(message == NULL) {
            puts("Sender: Message is Null");
        }

        // Send the message to remote socket
        if(sendto(socketDescriptor, message, strlen(message), 0, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
            puts("Sender: Failed to send");
            return NULL;
        }
        
        // Finish the program the trimmed message is "!" and clean up the memory
        if(strcmp(message,"!\n") == 0) {
            free(servinfo);
            free(message);
            List_free(senderList, ListManager_freeNode);
            pthread_mutex_destroy(&keyboardMutex);
            pthread_cond_destroy(&itemAvail);
            close(socketDescriptor);
            CancelThreads_receiverAndSceenCancel();
            break;
        }
        memset(message, 0, MSG_MAX_LEN);
        free(message);
    }
    return NULL;
}

void Sender_itemAvailSignal() {
    pthread_mutex_lock(&keyboardMutex);
    { 
        pthread_cond_signal(&itemAvail);
    }
    pthread_mutex_unlock(&keyboardMutex);
}

void Sender_init(char* name,char* port, int socket, pthread_mutex_t mutex) {
    ipAddress = name;
    remotePort = port;
    socketDescriptor = socket;
    keyboardMutex = mutex;
    senderList = ListManager_getsenderList();
    pthread_create(
        &pthreadSender,
        NULL,
        sendThread,
        NULL
    );
}

void Sender_join(void) {
    pthread_join(pthreadSender, NULL);
}

void Sender_cancel(void) {
    // Clean up the memory
    free(servinfo);
    List_free(senderList, ListManager_freeNode);
    close(socketDescriptor);
    pthread_cancel(pthreadSender);
    pthread_mutex_destroy(&keyboardMutex);
    pthread_cond_destroy(&itemAvail);
}