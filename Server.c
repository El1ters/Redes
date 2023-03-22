#include "Main.h"
#include "Server.h"

int Init_Server(Server info){
    struct addrinfo hints, *res;
    ssize_t n;
    int errcode,fd;

    fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1) exit(1);

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    errcode = getaddrinfo(info.ip,info.tcp,&hints,&res);

    if((errcode) != 0) exit(1);
    n = bind(fd,res->ai_addr,res->ai_addrlen);
    if(n == -1) exit(1);
    if(listen(fd,5) == -1) exit(1);/*Nao tenho a certeza do 2 argumento*/

    freeaddrinfo(res);
    return fd;
}

 
char *Give_List(char *string,int count){
    char *nodeslist = (char *)malloc(256);
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

    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt","59000",&hints,&res);
    if(errcode != 0)  exit(1);

    m = sendto(fd,string,count,0,res->ai_addr,res->ai_addrlen);
    if(m == -1)  exit(1);
    addrlen = sizeof(addr);
    m = recvfrom(fd,nodeslist,256,0,(struct sockaddr*)&addr,&addrlen);
    if(m == -1)  exit(1);

    write(1,"echo: ",6); write(1,nodeslist,m); write(1,"\n",1);
    freeaddrinfo(res);
    close(fd);
    return nodeslist;
}

void SendMessage(char *string,int count){
    char buffer[128];
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    ssize_t m, g ;
    int fd,errcode;

    fd = socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd == -1)  exit(1);
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; // IPv4
    hints.ai_socktype=SOCK_DGRAM; // UDP socket
    hints.ai_flags=AI_PASSIVE;

    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt","59000",&hints,&res);
    if(errcode != 0)  exit(1);

    m = sendto(fd,string,count,0,res->ai_addr,res->ai_addrlen);
    if(m == -1)  exit(1);
    addrlen = sizeof(addr);
    m = recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
    if(m == -1)  exit(1);

    write(1,"echo: ",6); write(1,buffer,m); write(1,"\n",1);
    freeaddrinfo(res);
    close(fd);
}

void SendExtern(int newfd, Nodes variables){ //Funçao que responde ao vizinho interno
    char tosend[50] = "EXTERN ";
    size_t n;
    strcat(tosend,variables.ext.id); strcat(tosend," ");
    strcat(tosend,variables.ext.ip); strcat(tosend," ");
    strcat(tosend,variables.ext.tcp); strcat(tosend,"\n");
    n = write(newfd, tosend, strlen(tosend));
}

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
    if(n==-1){
        fprintf(stderr,"error: %s\n",strerror(errno));
        exit(1);
    } 
    freeaddrinfo(res);
    return fd;
}