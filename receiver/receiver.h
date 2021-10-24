#ifndef _RECEIVER_H_
#define _RECEIVER_H_

void Receiver_init(int port, int socket, pthread_mutex_t displayMutex);
void Receiver_buffAvailSignal(void);
void Receiver_join(void);
void Receiver_cancel(void);

#endif
