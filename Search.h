#ifndef SEARCH
#define SEARCH

#include "Main.h"


Expedition* insertAtEnd(Expedition* head, char* dest, char* viz);
void printList(Expedition* head);
void freeList(Expedition* head);
void SendQuery(Nodes variables,char *msg,int sock_rec);

#endif