#include "Main.h"
#include "Interface.h"

void ConnectTejo(char *string, Server *info,Nodes *variables){
    int i = 0;
    static int primeiro = 0; /*0 significa que posso usar o comando "join", 1 significa o contrario*/
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
        char udp_message[10] = "NODES ";
        char compare[20] =  "NODESLIST ";


        if(primeiro == 0){
            strcpy(info->id,list[2]);
            strcpy(info->net,list[1]);
            strcpy(variables->id,list[2]);
        }

        strcat(udp_message,info->net);
        strcat(compare,info->net);
        strcat(compare,"\n");
        char *nodeslist = Give_List(udp_message,strlen(udp_message));

        if(strcmp(nodeslist,compare) == 0){
            strcpy(variables->bck,variables->id);
            Register(list,*info);
            primeiro = 1;
        }else if(primeiro == 0){
            join(list,*variables,*info,nodeslist);
            primeiro = 1;
        }
        free(nodeslist);
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
void Register(char list[6][50],Server info){
    char str1[10]="NODES 079";
    char str2[128]="REG ";

    strcat(str2,list[1]);
    strcat(str2," ");
    strcat(str2,list[2]);
    strcat(str2," ");
    strcat(str2,info.ip);
    strcat(str2," ");
    strcat(str2,info.tcp);
    SendMessage(str2,strlen(str2));
}

int join(char list[6][50],Nodes variables, Server info, char *nodeslist){
    char str2[128]="REG ";
    int fd;

    strcat(str2,list[1]);
    strcat(str2," ");
    strcat(str2,list[2]);
    strcat(str2," ");
    strcat(str2,info.ip);
    strcat(str2," ");
    strcat(str2,info.tcp);

    char id[3], ip[16], port[10];
    char *token = strtok(nodeslist,"\n");
    token = strtok(NULL,"\n");
    sscanf(token,"%s %s %s",id,ip,port);
    fd = EstablishConnection(ip,port);
    SendMessage(str2,strlen(str2));
    return fd;
}

void leave(Server info,int *i){
    char str[128] = "UNREG ";
    strcat(str,info.net); 
    strcat(str," ");
    strcat(str,info.id);
    if(*i == 1){
        SendMessage(str,strlen(str));
        (*i) = 0;
    }
}

