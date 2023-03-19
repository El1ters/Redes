#include "Main.h"

int main(int argc, char **argv){
    srand(time(NULL));
    int fd, newfd;
    fd_set rfds;
    int maxfd, counter;
    char string[128];
    socklen_t addrlen;
    struct sockaddr_in addr;
    Server info;
    Nodes variables;

    char buffer[128];
    int n;
    VerifyIP(argc,argv,&info); 

    /* Definir depois as variaveis dentro da struct server tejo_ip - ip do tejo tejo_udp - port do tejo
    Passar algures como argumento para as fuçoes de baixo*/

    fd = Init_Server(info);
    int number_on = 0;
    while(1){ 
        FD_SET(0,&rfds);
        FD_SET(3,&rfds);
        maxfd = fd;
        counter = select(maxfd + 1,&rfds, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)NULL);
        if(FD_ISSET(0,&rfds)){
            fgets(string,128,stdin);
            if(strcmp(string,"exit\n") == 0){
                close(fd);
                exit(0);
            } 
            if(strcmp(string,"\n") == 0) continue;
            ConnectTejo(string,&info,&variables);
            //printf("ext: id:%s ip:%s tcp:%s fd:%d\n",variables.ext.id,variables.ext.ip,variables.ext.tcp,variables.ext.fd);
        }
        if(FD_ISSET(3,&rfds)){
            addrlen = sizeof(addr);
            if((newfd = accept(fd,(struct sockaddr*)&addr,(socklen_t*)&addrlen)) == -1) exit(1);
            n = read(newfd,buffer,128); 
            write(1,"received: ",10); write(1,buffer,n);
            n = write(newfd,"bitch!\n",8);
            FD_SET(newfd,&rfds);
        }
        FD_ZERO(&rfds);
    } 
    return 0;
}