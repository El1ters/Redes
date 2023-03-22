#ifndef INTERFACE
#define INTERFACE

#include "Main.h"

void ConnectTejo(char *string, Server *info,Nodes *variables,int *maxfd);
void Register(char list[6][50],Server info);
int join(char list[6][50],Nodes variables,Server info,char *selected);
void leave(Server info,int *primeiro, Nodes Variables);
void PrintContacts(Nodes variables);


#endif