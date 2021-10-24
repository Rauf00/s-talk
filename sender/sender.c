#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "../sender/sender.h"
#include "../keyboard/keyboard.h"
#include "../list/listmanager.h"

#define MSG_MAX_LEN 1024

static char* remotePort;
static int socketDescriptor;
static pthread_t thread;
static List* senderList;
static char* message = NULL;
pthread_mutex_t keyboardMutex;
static pthread_cond_t itemAvail = PTHREAD_COND_INITIALIZER;

static char* ipAddress;

int status;

static struct addrinfo *servinfo;

void* sendThread(void* msgArg) {
    // Construct remote socket address
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;                   // connection may be from network
    hints.ai_socktype = SOCK_DGRAM;    // Host to Network long // local IP address
    hints.ai_flags = AI_PASSIVE;           // Host to Network short

    if(status = getaddrinfo(NULL, remotePort, &hints, &servinfo) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    while(1) {
        pthread_mutex_lock(&keyboardMutex);
        { 
            if (List_count(senderList) == 0)  {
                pthread_cond_wait(&itemAvail,&keyboardMutex);
            }
            // if there is an item in the list, trim it
            message = List_trim(senderList);
            // Signal producer that a new buffer is available
            Keyboard_buffAvailSignal();
        }
        pthread_mutex_unlock(&keyboardMutex);

        // Send the message to remote socket
        sendto(socketDescriptor, 
            message, strlen(message), 0, servinfo->ai_addr, servinfo->ai_addrlen);

        if(strcmp(message,"!\n") == 0) {
            puts("PROGRAM SHUTDOWN");
            free(servinfo);
            exit(1);
        }
        free(message);
        message = NULL;
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
        &thread,
        NULL,
        sendThread,
        NULL
    );
}

void Sender_shutdown(void) {
    pthread_cancel(thread);
    pthread_join(thread, NULL);
}