#include "Main.h"

int main(int argc, char **argv){
    int fd, newfd;
    fd_set rfds;
    int maxfd, counter;
    char string[128];
    socklen_t addrlen;
    struct sockaddr_in addr;
    char IP[25] = "127.0.0.1", TCP[10] = "8080";
    char id[3], net[4];

    fd = Init_Server(IP,TCP);
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

            /*Alocar char list[6][50], fazer o strtok, passar como argumento na funçao debaixo*/

            ConnectTejo(string,strlen(string),IP,TCP,fd,id,net);
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
