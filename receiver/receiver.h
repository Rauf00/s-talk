#ifndef _RECEIVER_H_
#define _RECEIVER_H_

void Receiver_init(char* rxMessage, int port, int socket, pthread_mutex_t display_mutex);
void Receiver_buffAvailSignal();
void Receiver_shutdown(void);

#endif
