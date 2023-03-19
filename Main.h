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
#include <time.h>

/*Estrutura que tem a informaçao de um vizinho*/
typedef struct neighbors{
    int fd;
    char id[3];
    char ip[16];
    char tcp[10];
}Neighbors;

/*Estrutura que tem a informação do servidor*/
typedef struct server{
    char ip[16];
    char id[3];
    char net[4];   
    char tcp[10];
}Server;

/*Estrutura que tem a informaçao do backup e dos seus vizinhos internos e externo*/
typedef struct nodes{
    char id[3];
    Neighbors bck;
    Neighbors intr[99];
    Neighbors ext;
}Nodes;

extern int numero_ancoras;

#include "Interface.h"
#include "Server.h"
#include "Verifications.h"

#endif
