#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "../list/list.h"

void Keyboard_init(pthread_mutex_t keyboardMutex);
void Keyboard_buffAvailSignal(void);
void Keyboard_join(void);
void Keyboard_cancel(void);

#endif