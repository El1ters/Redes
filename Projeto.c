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



void ConnectTejo(char *string,int count);
int Init_Server();


int main(int argc, char **argv){
    int fd, newfd;
    fd_set rfds;
    int maxfd, counter;
    char string[128];
    socklen_t addrlen;
    struct sockaddr_in addr;

    fd = Init_Server();
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
            ConnectTejo(string,strlen(string));
        }

        if(FD_ISSET(3,&rfds)){
            addrlen = sizeof(addr);
            if((newfd = accept(fd,(struct sockaddr*)&addr,(socklen_t*)&addrlen)) == -1)exit(1);
            FD_SET(newfd,&rfds);
            printf("bitch\n");
        }
        
        FD_ZERO(&rfds);
    }
    
    return 0;
}

void ConnectTejo(char *string,int count){
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    char buffer[128], key[10];


    sscanf(string,"%s",key);
    if (strcmp(key, "join") == 0)
    {
    }
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
    }
    else if (strcmp(key, "leave") == 0)
    {
    }

    fd = socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd == -1) /*error*/ exit(1);
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; // IPv4
    hints.ai_socktype=SOCK_DGRAM; // UDP socket
    hints.ai_flags=AI_PASSIVE;

    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt","59000",&hints,&res);
    if(errcode != 0) /*error*/ exit(1);

        n = sendto(fd,string,count,0,res->ai_addr,res->ai_addrlen);
        if(n == -1) /*error*/ exit(1);
        addrlen = sizeof(addr);
        n = recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
        if(n == -1) /*error*/ exit(1);
        write(1,"echo: ",6); write(1,buffer,n);
    freeaddrinfo(res);
    close(fd);
}

/*void User(char *string,int count){


}*/

int Init_Server(){
    struct addrinfo hints, *res;
    ssize_t n;
    int errcode,fd;

    fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1) exit(1);

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    errcode = getaddrinfo("127.0.0.1","8080",&hints,&res);

    if((errcode) != 0) exit(1);
    n = bind(fd,res->ai_addr,res->ai_addrlen);
    if(n == -1) exit(1);
    if(listen(fd,5) == -1) exit(1);/*Nao tenho a certeza do 2 argumento*/

    freeaddrinfo(res);
    return fd;
}