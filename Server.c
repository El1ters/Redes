#include "Main.h"
#include "Server.h"

/********************************************************************************************************************************
int Init_Server(Server info)

Inicializa um servidor TCP numa porta específica.

    - Entrada:
        -info: Estrutura do tipo Server como as informações do servidor como o IP e a porta.

    - Saída:
        - Retorna o fd do socket do servidor
********************************************************************************************************************************/
int Init_Server(Server info){
    struct addrinfo hints, *res;
    ssize_t n;
    int errcode,fd;

    // Criar um socket TCP
    fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1) exit(1);

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    errcode = getaddrinfo(info.ip,info.tcp,&hints,&res);

    if((errcode) != 0) exit(1);
    // Conectar o socket à porta e endereço especificados
    n = bind(fd,res->ai_addr,res->ai_addrlen);
    if(n == -1) exit(1);
    if(listen(fd,5) == -1) exit(1);/*Nao tenho a certeza do 2 argumento*/

    freeaddrinfo(res);
    return fd;
}

/********************************************************************************************************************************
char *Give_List(char *string,int count)

Envia um pedido para o servidor "tejo.tecnico.ulisboa.pt" na porta "59000",
com uma string, utilizando o protocolo UDP. A função espera por uma resposta 
do servidor e a armazena no buffer.

    - Entrada:
        -string: string a ser enviada como pedido para o servidor
        -count: tamanho da string em bytes

    - Saída:
        - Retorna um ponteiro para o buffer que contém a resposta do servidor, ou NULL em caso de erro.
********************************************************************************************************************************/
char *Give_List(char *string,int count){
    char *nodeslist = (char *)malloc(1000);
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    ssize_t m;
    int fd,errcode;

    fd = socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd == -1)  exit(1);
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; // IPv4
    hints.ai_socktype=SOCK_DGRAM; // UDP socket
    hints.ai_flags=AI_PASSIVE;

    errcode = getaddrinfo(regIP,regUDP,&hints,&res);
    if(errcode != 0)  exit(1);

    m = sendto(fd,string,count,0,res->ai_addr,res->ai_addrlen);
    if(m == -1)  exit(1);
    addrlen = sizeof(addr);
    m = recvfrom(fd,nodeslist,1000,0,(struct sockaddr*)&addr,&addrlen);
    if(m == -1)  exit(1);

    write(1,"echo: ",6); write(1,nodeslist,m); write(1,"\n",1);
    freeaddrinfo(res);
    close(fd);
    return nodeslist;
}

/********************************************************************************************************************************
void SendMessage(char *string,int count){

Envia uma mensagem para o tejo através de uma ligação UDP e aguarda uma resposta.

    - Entrada:
        -string: Ponteiro para o buffer de caracteres que contém a mensagem a ser enviada.
        -count: Número de bytes da mensagem a ser enviada.

    - Saída:
        - void, logo não retorna nenhum valor
********************************************************************************************************************************/
void SendMessage(char *string,int count){
    char *buffer = (char *)malloc(1000);
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    ssize_t m, g ;
    int fd,errcode;

    fd = socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd == -1)  exit(1);
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(regIP,regUDP,&hints,&res);
    if(errcode != 0)  exit(1);

    m = sendto(fd,string,count,0,res->ai_addr,res->ai_addrlen);
    if(m == -1)  exit(1);
    addrlen = sizeof(addr);
    m = recvfrom(fd,buffer,1000,0,(struct sockaddr*)&addr,&addrlen);
    if(m == -1)  exit(1);

    write(1,"echo: ",6); write(1,buffer,m); write(1,"\n",1);
    free(buffer);
    freeaddrinfo(res);
    close(fd);
}

/********************************************************************************************************************************
void SendExtern(int newfd, Nodes variables){

Envia mensagem com informações do nó externo para um vizinho interno, sendo a sua resposta à entrada do mesmo

    - Entrada:
        -newfd: fd da conexão com o vizinho interno
        -variables: estrutura de nós que contém as informações do nó externo e dos vizinhos internos

    - Saída:
        - void, logo não retorna nenhum valor
********************************************************************************************************************************/
void SendExtern(int newfd, Nodes variables){ //Funçao que responde ao vizinho interno
    char tosend[50] = "EXTERN ";
    ssize_t n;
    strcat(tosend,variables.ext.id); strcat(tosend," ");
    strcat(tosend,variables.ext.ip); strcat(tosend," ");
    strcat(tosend,variables.ext.tcp); strcat(tosend,"\n");
    n = write(newfd, tosend, strlen(tosend));
}

/********************************************************************************************************************************
void SendNew(int fd,Server info)

Função responsável por enviar a mensagem "NEW" para um nó específico, informando as informações do servidor atual.

    - Entrada:
        -fd: inteiro que representa o fd do socket a ser utilizado para enviar a mensagem.
        -info: estrutura do tipo Server com as informações do servidor atual (id, ip e tcp).

    - Saída:
        - void, logo não retorna nenhum valor
********************************************************************************************************************************/
void SendNew(int fd,Server info){
    ssize_t n;
    char aux[25] = {};    
    strcat(aux,"NEW ");
    strcat(aux,info.id); strcat(aux," ");
    strcat(aux,info.ip); strcat(aux," ");
    strcat(aux,info.tcp); strcat(aux,"\n");

    n = write(fd,aux,strlen(aux));
    if(n == -1) exit(1);
}

/********************************************************************************************************************************
int EstablishConnection(char *ip,char *tcp, Server info)

Função para estabelecer a conexão TCP com um nó.
 
    - Entrada:
        - ip: endereço IP do nó a ser conectado
        - tcp: porta TCP do nó a ser conectado
        - info : Estrutura de dados que contém as informações do servidor.

    - Saída:
        - o fd da conexão estabelecida
********************************************************************************************************************************/
int EstablishConnection(char *ip,char *tcp, Server info){ //Funçao que estabelece a ligaçao a um no e lhe manda o NEW
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res; 
    struct sockaddr_in addr;
    char buffer[128];

    fd = socket(AF_INET,SOCK_STREAM, 0);
    if(fd == -1) exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    errcode = getaddrinfo(ip,tcp, &hints, &res);
    if(errcode != 0) exit(1);

    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if(n == -1){
        fprintf(stderr,"error: %s\n",strerror(errno));
        exit(1);
    } 
    freeaddrinfo(res);
    return fd;
}

/********************************************************************************************************************************
void HandleNode(Nodes *variables, int *maxfd, Server info)

Função responsável por lidar com a perda de conexão com o nó externo.
 
    - Entrada:
        - variables: estrutura com informações sobre a rede de comunicação e nós
        - maxfd: Ponteiro para o valor máximo dos fds
        - info : Estrutura de dados que contém as informações do servidor.

    - Saída:
        - void, logo não retorna nenhum valor
********************************************************************************************************************************/
void HandleNode(Nodes *variables, int *maxfd, Server info)
{
    printf("Perdeu conexao com o externo\n");
    close(variables->ext.fd);
    FD_CLR(variables->ext.fd, &rfds);
    if (strcmp(variables->id, variables->bck.id) == 0)
    {
        int viz = 0;
        // Para ir buscar o primeiro vizinho interno
        for (viz; viz != 99; viz++)
            if (variables->intr[viz].fd != -1)
                break;

        if (variables->intr[viz].fd != -1 && viz != 99)
        {
            strcpy(variables->ext.id, variables->intr[viz].id);
            strcpy(variables->ext.ip, variables->intr[viz].ip);
            strcpy(variables->ext.tcp, variables->intr[viz].tcp);
            variables->ext.fd = variables->intr[viz].fd;
            for (int j = 0; j != 99; j++)
            {
                if (variables->intr[j].fd != -1)
                {
                    SendExtern(variables->intr[j].fd, *variables);
                }
            }
            memset(&variables->intr[viz], 0, sizeof(variables->intr[viz]));
            variables->intr[viz].fd = -1;
        }
        else
        {
            first_node = 1;
            strcpy(variables->ext.id, variables->id);
            strcpy(variables->ext.ip, info.ip);
            strcpy(variables->ext.tcp, info.tcp);
            variables->ext.fd = -1;
        }
    }
    else
    {
        variables->ext.fd = EstablishConnection(variables->bck.ip, variables->bck.tcp, info);
        if (variables->ext.fd > *maxfd)
            *maxfd = variables->ext.fd;
        FD_SET(variables->ext.fd, &rfds);
        strcpy(variables->ext.id, variables->bck.id);
        strcpy(variables->ext.ip, variables->bck.ip);
        strcpy(variables->ext.tcp, variables->bck.tcp);
        variables->head = insertAtEnd(variables->head,variables->ext.id,variables->ext.id);
        SendNew(variables->ext.fd, info);
        for (int k = 0; k != 99; k++){
            if (variables->intr[k].fd != -1)
                SendExtern(variables->intr[k].fd, *variables);
        }
    }
    
}
