#ifndef SERVER
#define SERVER

#include "Main.h"

int Init_Server(char *IP,char *TCP);
char *Give_List(char*string,int count);
void SendMessage(char *string,int count);

#endif