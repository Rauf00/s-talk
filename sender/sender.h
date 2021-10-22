#ifndef _SENDER_H_
#define _SENDER_H_

void Sender_init(int port,  int socket, pthread_mutex_t keyboardMutex);
void Sender_itemAvailSignal(void);
void Sender_shutdown(void);

#endif
