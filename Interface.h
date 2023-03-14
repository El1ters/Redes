#ifndef INTERFACE
#define INTERFACE

#include "Main.h"

void ConnectTejo(char *string,int count,char *IP,char *TCP,int fd, char *id,char *net);
void join(char list[6][50],char *ID, char* TCP);
void leave(int fd,char *id,char *net);




#endif