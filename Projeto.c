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



void ConnectTejo(char *string,int count,char *IP,char *TCP);
int Init_Server(char *IP,char *TCP);
void join(char list[6][50],char *ID, char* TCP);
char *Give_List(char*string,int count);
void SendMessage(char *string,int count);

int main(int argc, char **argv){
    int fd, newfd;
    fd_set rfds;
    int maxfd, counter;
    char string[128];
    socklen_t addrlen;
    struct sockaddr_in addr;
    char IP[25] = "127.0.0.1", TCP[10] = "8080";

    fd = Init_Server(IP,TCP);
    while(1){
        FD_SET(0,&rfds);
        FD_SET(3,&rfds);
        maxfd = fd;
        counter = select(maxfd + 1,&rfds, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)NULL);
        if(counter <= 0) exit(1);
        if(FD_ISSET(0,&rfds)){
            fgets(string,128,stdin);
            if(strcmp(string,"exit\n") == 0){
                close(fd);
                exit(0);
            } 
            ConnectTejo(string,strlen(string),IP,TCP);
        }

        if(FD_ISSET(3,&rfds)){
            addrlen = sizeof(addr);
            if((newfd = accept(fd,(struct sockaddr*)&addr,(socklen_t*)&addrlen)) == -1)exit(1);
            FD_SET(newfd,&rfds);
        }
        
        FD_ZERO(&rfds);
    }
    
    return 0;
}

void ConnectTejo(char *string,int count,char *IP,char *TCP){
    int fd, errcode, i = 0;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    char /*buffer[128],*/list[6][50] = {}, *token;

    token = strtok(string," ");
    while(token != NULL){
        strcpy(list[i],token);
        token = strtok(NULL," ");
        i++;
    }
    token = strtok(list[--i],"\n");
    strcpy(list[i],token);

    if (strcmp(list[0], "join") == 0){
        join(list,IP,TCP);
    }/*
    else if (strcmp(key, "djoin") == 0)
    {
    }
    else if ((strcmp(key, "create") == 0))
    {
    }
    else if ((strcmp(key, "delete") == 0))
    {
    }
    else if (strcmp(key, "get") == 0)
    {
    }
    else if (strcmp(key, "show") == 0)
    {
        if (strcmp(key2, "topology") == 0)
        {
        }
        else if (strcmp(key2, "names"))
        {
        }
        else if (strcmp(key2, "routing"))
        {
        }
    }*/
    else if (strcmp(list[0], "leave") == 0){

    }

    /*fd = socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd == -1)  exit(1);
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; // IPv4
    hints.ai_socktype=SOCK_DGRAM; // UDP socket
    hints.ai_flags=AI_PASSIVE;

    printf("%s\n",string);
    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt","59000",&hints,&res);
    if(errcode != 0)  exit(1);

        n = sendto(fd,string,count,0,res->ai_addr,res->ai_addrlen);
        if(n == -1)  exit(1);
        addrlen = sizeof(addr);
        n = recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
        if(n == -1)  exit(1);
        write(1,"echo: ",6); write(1,buffer,n);
    freeaddrinfo(res);
    close(fd);
    */
}

/*void User(char *string,int count){


}*/

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

void join(char list[6][50],char *IP, char* TCP){
    int fd2, errcode2;
    char str1[10]="NODES 079";
    char str2[128]="REG ";
    char *nodeslist;

    strcat(str2,list[1]);
    strcat(str2," ");
    strcat(str2,list[2]);
    strcat(str2," ");
    strcat(str2,IP);
    strcat(str2," ");
    strcat(str2,TCP);

    nodeslist = Give_List(str1,strlen(str1));
    if(strcmp(nodeslist, "NODESLIST 079\n") == 0){
        SendMessage(str2,strlen(str2));
    }/*else{
        Conectar a um nó
    }*/
    free(nodeslist);
}

char *Give_List(char *string,int count){
    char *nodeslist = (char*)malloc(256);
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

    write(1,"echo: ",6); write(1,buffer,m);
    freeaddrinfo(res);
    close(fd);
}