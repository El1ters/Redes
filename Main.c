#include "Main.h"

int first_node = 0;
fd_set rfds;

int main(int argc, char **argv){
    srand(time(NULL));
    int fd, newfd;
    int maxfd, counter;
    char string[128];
    socklen_t addrlen;
    struct sockaddr_in addr;
    addrlen = sizeof(addr);
    Server info;
    Nodes variables;
    variables.num_names = 0;
    int number_on = 0;
    for(int k = 0;k != 99; k++){
        variables.intr[k].fd = -1;
    }
    variables.ext.fd = -1;    
    VerifyIP(argc,argv,&info); //info->ip e info->tcp fica guardado
    FD_ZERO(&rfds);
    fd = Init_Server(info);
    if(fd > maxfd)
        maxfd = fd;
    while(1){
        FD_SET(STDIN_FILENO,&rfds);
        FD_SET(fd,&rfds);
        for(int j = 0;j != 99;j++){
            if(variables.intr[j].fd != -1)
                FD_SET(variables.intr[j].fd,&rfds);
        }
        FD_SET(variables.ext.fd,&rfds);
        counter = select(maxfd + 1,&rfds, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)NULL);
        if(FD_ISSET(STDIN_FILENO,&rfds)){
            bzero(string,128);
            fgets(string,128,stdin);
            if(strcmp(string,"exit\n") == 0){
                close(fd);
                exit(0);
            } 
            if(strcmp(string,"\n") == 0) 
                continue;
            ConnectTejo(string,&info,&variables,&maxfd);
        }

        if(FD_ISSET(fd,&rfds)){
            if((newfd = accept(fd,(struct sockaddr*)&addr,(socklen_t*)&addrlen)) == -1) exit(1);
                
                int n;
                if(newfd > maxfd)
                    maxfd = newfd;
                
                if(first_node == 1){
                    printf("nova ancora\n");
                    variables.ext.fd = newfd;
                    first_node++;
                    FD_SET(variables.ext.fd,&rfds);
                }else{
                    printf("novo interno\n");
                    for(int k = 0;k != 99;k++){
                        if(variables.intr[k].fd == -1){
                            variables.intr[k].fd = newfd;
                            FD_SET(variables.intr[k].fd,&rfds);
                            break;
                        }
                    }
                    number_on++;
                }
        }

        for(int j = 0;j != number_on;j++){
            if(FD_ISSET(variables.intr[j].fd,&rfds)){
                int n;
                char buffer[128] = {};
                char message[4][20] = {};
                n = read(variables.intr[j].fd,buffer,128);
                if(n == 0){
                    printf("Perdeu conexao com o interno\n");
                    close(variables.intr[j].fd);
                    FD_CLR(variables.intr[j].fd,&rfds);
                    memset(&variables.intr[j],0,sizeof(variables.intr[j]));
                    variables.intr[j].fd = -1;
                    break;
                }else{
                    sscanf(buffer,"%s %s %s %s",message[0],message[1],message[2],message[3]);
                    write(1,"received: ",11); write(1,buffer,strlen(buffer) + 1);
                    if(strcmp(message[0],"NEW") == 0){
                        strcpy(variables.intr[j].id,message[1]); strcpy(variables.intr[j].ip,message[2]); 
                        strcpy(variables.intr[j].tcp,message[3]);
                        SendExtern(variables.intr[j].fd,variables);
                    }
                }
            }
        }

        if(FD_ISSET(variables.ext.fd,&rfds)){
            int n;
            char buffer[128] = {};
            char message[4][20] = {};
            n = read(variables.ext.fd,buffer,128);

            if(n == 0){
                printf("Perdeu conexao com o externo\n");
                close(variables.ext.fd);
                FD_CLR(variables.ext.fd,&rfds);

                if(strcmp(variables.id,variables.bck.id) == 0){
                    first_node = 1;
                    strcpy(variables.ext.id,variables.id);
                    strcpy(variables.ext.ip,info.ip);
                    strcpy(variables.ext.tcp,info.tcp);
                    variables.ext.fd = -1;
                } else {
                    //if clause que verifica se o seu bck igual ao id (pois isto significa que a ancora pedeu conexao com o outro), 
                    //se for esse o caso tem de promover um dos internos a seu externo para definilo como ancora
                    variables.ext.fd = EstablishConnection(variables.bck.ip,variables.bck.tcp,info);
                    if(variables.ext.fd > maxfd)
                        maxfd = variables.ext.fd;
                    FD_SET(variables.ext.fd,&rfds);
                    strcpy(variables.ext.id,variables.bck.id); strcpy(variables.ext.ip,variables.bck.ip); strcpy(variables.ext.tcp,variables.bck.tcp);
                    SendNew(variables.ext.fd,info);
                }
                for(int k = 0;k != 99;k++){
                    if(variables.intr[k].fd != -1)
                        SendExtern(variables.intr[k].fd,variables);
                }
            }else{ 
                sscanf(buffer,"%s %s %s %s",message[0],message[1],message[2],message[3]);
                write(1,"received: ",11); write(1,buffer,strlen(buffer) + 1);
                if(strcmp(message[0],"NEW") == 0){
                    strcpy(variables.ext.id,message[1]); strcpy(variables.ext.ip,message[2]); strcpy(variables.ext.tcp,message[3]);
                    SendExtern(variables.ext.fd,variables);
                    FD_SET(variables.ext.fd,&rfds);
                } else if(strcmp(message[0],"EXTERN") == 0){
                    strcpy(variables.bck.id,message[1]); strcpy(variables.bck.ip,message[2]); strcpy(variables.bck.tcp,message[3]);
                }
            }
        }
    } 
    return 0;
}