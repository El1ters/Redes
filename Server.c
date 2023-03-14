#include "Main.h"
#include "Server.h"

int Init_Server(char *IP,char *TCP){
    struct addrinfo hints, *res;
    ssize_t n;
    int errcode,fd;

    fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1) exit(1);

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    errcode = getaddrinfo(IP,TCP,&hints,&res);

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
    m = recvfrom(fd,nodeslist,256,0,(struct sockaddr*)&addr,&addrlen);
    if(m == -1)  exit(1);

    write(1,"echo: ",6); write(1,nodeslist,m);
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