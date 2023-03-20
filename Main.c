#include "Main.h"

int first_node = 0;

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
    FD_ZERO(&rfds);
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
                int n;
                maxfd = newfd;
                if(first_node == 1){
                    variables.ext.fd = newfd;
                    number_on++;
                    first_node++;
                    FD_SET(variables.ext.fd,&rfds);
                }
                //char buffer[50];
                //variables.intr[number_on].fd = newfd;
                //number_on++;
                //n = read(newfd,buffer,50);
                //printf("buffer: %s\n",buffer);
                //char *intern;
                //intern = SendExtern(newfd,&variables);
                //n = write(newfd,"oi\n",3);
                //sscanf(buffer,"NEW %s %s %s",variables.ext.id,variables.ext.ip,variables.ext.tcp);
                /*char * intern;
                intern = SendExtern(newfd,&variables);
                printf("ola: %s\n",intern);*/
                //manda o EXTERN e recebe informaçao do interno
            FD_SET(newfd,&rfds);
        }
        
        // if(FD_ISSET(newfd,&rfds)){
        //     ssize_t n;
        //     char buffer[128];
        //     char message[4][20] = {};
        //     n = read(newfd,buffer,128);
        //     sscanf(buffer,"%s %s %s %s",message[0],message[1],message[2],message[3]);

        //     printf("%s %s\n",message[1],variables.ext.id);
        //     if(strcmp(message[1],variables.ext.id) == 0)
        //         printf("primeira conexao");

        // }

        for(int j = 0;j != number_on;j++){
            if(FD_ISSET(variables.intr[j].fd,&rfds)){
                ssize_t n;
                char buffer[128];
                char message[4][20] = {};
                n = read(variables.intr[j].fd,buffer,128);
                sscanf(buffer,"%s %s %s %s",message[0],message[1],message[2],message[3]);

                if(strcmp(message[0],"NEW") == 0){
                    SendExtern(variables.intr[j].fd,variables);
                }
            }
        }

    } 
    return 0;
}