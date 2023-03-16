#ifndef MAIN
#define MAIN

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

typedef struct server{
    char ip[16];
    char id[3];
    char net[4];
    char tcp[10];
}Server;

typedef struct nodes{
    char id[3];
    char bck[3];
    Server intr[99];
    Server ext;
}Nodes;

#include "Interface.h"
#include "Server.h"
#include "Verifications.h"

#endif
