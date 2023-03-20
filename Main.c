#include "Main.h"
int numero_ancoras = 0;

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

    int n;
    VerifyIP(argc,argv,&info); //info->ip e info->tcp fica guardado

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
        }
        if(FD_ISSET(3,&rfds)){
            addrlen = sizeof(addr);
            if((newfd = accept(fd,(struct sockaddr*)&addr,(socklen_t*)&addrlen)) == -1) exit(1);
            if(numero_ancoras == 1) {
                numero_ancoras = 2;
                int n;
                char buffer[50];
                //n = read(newfd,buffer,50);
                //printf("buffer: %s\n",buffer);
                char *intern;
                intern = SendExtern(newfd,&variables);
                //n = write(newfd,"oi\n",3);
                sscanf(buffer,"NEW %s %s %s",variables.ext.id,variables.ext.ip,variables.ext.tcp);
            }else{
                char * intern;
                intern = SendExtern(newfd,&variables);
                printf("ola: %s\n",intern);
            }
                //manda o EXTERN e recebe informaçao do interno
            FD_SET(newfd,&rfds);
        }

        printf("numero %d\n",numero_ancoras);
        FD_ZERO(&rfds);
    } 
    return 0;
}