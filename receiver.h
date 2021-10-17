#ifndef _RECEIVER_H_
#define _RECEIVER_H_

void Receiver_init(char* rxMessage, int port, int socket);

void Receiver_changeDynamicMessage(char* rxMessage);

void Receiver_shutdown(void);

#endif
