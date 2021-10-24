#ifndef _SCREEN_H_
#define _SCREEN_H_

void Screen_init(pthread_mutex_t displayMutex);
void Screen_itemAvailSignal(void);
void Screen_join(void);
void Screen_cancel(void);

#endif