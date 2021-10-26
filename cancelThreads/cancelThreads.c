#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "../receiver/receiver.h"
#include "../sender/sender.h"
#include "../keyboard/keyboard.h"
#include "../screen/screen.h"

void CancelThreads_receiverAndSceenCancel() {
    Receiver_cancel();
    Screen_cancel();
}

void CancelThreads_keyboardAndSenderCancel() {
    Keyboard_cancel();
    Sender_cancel();
}