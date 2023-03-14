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



int main(){
    char buffer[128], string[128];
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

    while(1){
        fgets(string,128,stdin);
        m = sendto(fd,string,strlen(string),0,res->ai_addr,res->ai_addrlen);
        if(m == -1)  exit(1);
        addrlen = sizeof(addr);
        m = recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
        if(m == -1)  exit(1);

        write(1,"echo: ",6); write(1,buffer,m);
        
    }
    freeaddrinfo(res);
    close(fd);
    return 0;
}






