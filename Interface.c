#include "Main.h"
#include "Interface.h"

void ConnectTejo(char *string, Server *info){
    int i = 0;
    static int primeiro = 0;
    char list[6][50] = {}, *token;
    token = strtok(string," ");
    while(token != NULL){
        if(token != NULL)
            strcpy(list[i],token);
        token = strtok(NULL," ");
        i++;
    }
    token = strtok(list[--i],"\n");

    if (strcmp(list[0], "join") == 0 && strlen(list[1]) == 3 && strlen(list[2]) == 2 && strlen(list[3]) == 0){
        strcpy(info->id,list[2]);
        strcpy(info->net,list[1]);
        join(list,*info,&primeiro);
    }/*
    else if (strcmp(key, "djoin") == 0 && primeiro != 0)
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
    else if (strcmp(list[0], "leave") == 0 && strlen(list[1]) == 0){
        leave(*info,&primeiro);
    }
}


void join(char list[6][50],Server info, int *i){
    char str1[10]="NODES 079";
    char str2[128]="REG ";
    char *nodeslist;

    strcat(str2,list[1]);
    strcat(str2," ");
    strcat(str2,list[2]);
    strcat(str2," ");
    strcat(str2,info.ip);
    strcat(str2," ");
    strcat(str2,info.tcp);

    nodeslist = Give_List(str1,strlen(str1));
    if(strcmp(nodeslist, "NODESLIST 079\n") == 0){
        SendMessage(str2,strlen(str2));
        *i = 1;
    }/*else{
        Conectar a um nó
    }*/
    free(nodeslist);
}

void leave(Server info,int *i){
    char str[128] = "UNREG ";
    strcat(str,info.net); 
    strcat(str," ");
    strcat(str,info.id);
    if(*i == 1){
        SendMessage(str,strlen(str));
        (*i)++;
    }
}