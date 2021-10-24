#ifndef _SENDER_H_
#define _SENDER_H_

void Sender_init(char* name,char* port,  int socket, pthread_mutex_t keyboardMutex);
void Sender_itemAvailSignal(void);
void Sender_join(void);
void Sender_cancel(void);

#endif
