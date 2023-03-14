#include "Main.h"
#include "Interface.h"

void ConnectTejo(char *string,int count,char *IP,char *TCP,int fd, char *id,char *net){
    int i = 0;
    char list[6][50] = {}, *token;
    token = strtok(string," ");
    while(token != NULL){
        if(token != NULL)
            strcpy(list[i],token);
        token = strtok(NULL," ");
        i++;
    }
    token = strtok(list[--i],"\n");
    strcpy(list[i],token);

    if (strcmp(list[0], "join") == 0){
        strcpy(id,list[2]);
        strcpy(net,list[1]);
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
        leave(fd,id,net);
    }
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

void leave(int fd,char *id,char *net){
    char str[128] = "UNREG ";
    strcat(str,net);
    strcat(str," ");
    strcat(str,id);
    SendMessage(str,strlen(str));
    close(fd);
}