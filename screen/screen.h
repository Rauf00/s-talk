#ifndef _SCREEN_H_
#define _SCREEN_H_

void Screen_init(pthread_mutex_t display_mutex);
void Screen_itemAvailSignal();
void Screen_shutdown(void);

#endif