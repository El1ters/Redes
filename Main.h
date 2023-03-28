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

/*E Estrutura que contém informações sobre um vizinho, 
como o seu fd, ID, endereço IP e sua porta TCP.*/
typedef struct neighbors{
    int fd;
    char id[3];
    char ip[16];
    char tcp[10];
}Neighbors;

/*Estrutura que tem a informação sobre o servidor, 
como o seu endereço IP. o ID, a rede e a porta TCP.*/
typedef struct server{
    char ip[16];
    char id[3];
    char net[4];   
    char tcp[10];
}Server;
/* Estrutura que representa um caminho percorrido por uma mensagem até ao destino, 
consistindo no destino, o vizinho mais próximo de modo a percorrer esse caminho
 e um ponteiro para a próxima expedição na cadeia.*/
 typedef struct expedition{
    char dest[3];
    char viz[3];
    struct expedition *next;
}Expedition;

/*Estrutura que tem a informaçao do backup e dos vizinhos internos e externo, 
como o ID, a lista de nomes, o número de nomes, a lista de caminhos de expedição 
 o vizinho de backup, os vizinhos internos e o vizinho externo.*/
typedef struct nodes{
    char names[100][100];
    int num_names;
    char id[3];
    Expedition *head;
    Neighbors bck;
    Neighbors intr[99];
    Neighbors ext;
}Nodes;

extern int first_node; // Variável globar que deteta se o nó atual é o primeiro da rede.
extern fd_set rfds; // É usado em conjunto com select() para monitorar a entrada e saída de fd's.

#include "Interface.h"
#include "Server.h"
#include "Verifications.h"
#include "Search.h"

#endif
