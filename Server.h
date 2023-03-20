#ifndef SERVER
#define SERVER

#include "Main.h"

int Init_Server(Server info);
char *Give_List(char* string,int count);
void SendMessage(char *string,int count);
int EstablishConnection(char *ip,char *tcp,Server info);
char* SendExtern(int newfd, Nodes *variables);
char* SendNew(int fd,Server info);
void SetAncor(Server info);

#endif