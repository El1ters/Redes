#ifndef SERVER
#define SERVER

#include "Main.h"

int Init_Server(Server info);
char *Give_List(char* string,int count);
void SendMessage(char *string,int count);
int EstablishConnection(char *ip,char *tcp,Server info);
void SendExtern(int newfd, Nodes variables);
void SendNew(int fd,Server info);
void HandleNode(Nodes *variables, int *maxfd, Server info);

#endif