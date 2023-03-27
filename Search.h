#ifndef SEARCH
#define SEARCH

#include "Main.h"


Expedition* insertAtEnd(Expedition* head, char* dest, char* viz);
void printList(Expedition* head);
void freeList(Expedition* head);
void SendQuery(Nodes variables,char *msg,int sock_rec,char *txt,char *dest,char *origin);
void BackToSender(char *string, Nodes variables,char *dest,char *txt,char *origin);
char *GetNext(Nodes variables,char *dest,char *origin);
void Withdraw(Nodes variables,int sock_rec,char *msg);
Expedition *remove_node(Nodes variables, int *no);

#endif