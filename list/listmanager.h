#ifndef _LISTMANAGER_H_
#define _LISTMANAGER_H_

#include "list.h"

int ListManager_create();

//getters and setters
List* ListManager_getsenderList();
List* ListManager_getreceiverList();


#endif