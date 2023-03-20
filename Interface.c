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
            strcpy(info->id,list[2]); //Guardar o ID do meu server na info
            strcpy(info->net,list[1]); //Guardar o net na info
            strcpy(variables->id,list[2]); //Guardar o ID na struct pedida no projeto
        }

        strcat(udp_message,info->net);
        strcat(compare,info->net);
        strcat(compare,"\n");
        char *nodeslist = Give_List(udp_message,strlen(udp_message));

        if(strcmp(nodeslist,compare) == 0){
            strcpy(variables->bck.id,info->id); strcpy(variables->bck.ip,info->ip); strcpy(variables->bck.tcp,info->tcp);
            strcpy(variables->ext.id,info->id); strcpy(variables->ext.ip,info->ip); strcpy(variables->ext.tcp,info->tcp); 
            Register(list,*info);
            numero_ancoras = 1;
            primeiro = 1;
            //printf("Lista vazia ->ancoras: %d\n",numero_ancoras);
        }else if(primeiro == 0){
            char *selected = verify_id_is_used(nodeslist);
            //printf("ancoras %d\n",numero_ancoras);
            sscanf(selected,"%s %s %s",variables->ext.id,variables->ext.ip,variables->ext.tcp); //Guardar as informaçoes do externo
            if(numero_ancoras == 2){
                strcpy(variables->bck.id,info->id); strcpy(variables->bck.ip,info->ip); strcpy(variables->bck.tcp,info->tcp);
                numero_ancoras++;
            }
            variables->ext.fd = join(list,*variables,*info,selected);
            primeiro = 1;
            free(selected);
        }
        free(nodeslist);
    }
    else if (strcmp(list[0], "djoin") == 0 && strlen(list[1]) == 3 && strlen(list[2]) == 2 && primeiro == 0){
        if (strcmp(list[2],list[3]) == 0){
            strcpy(variables->bck.id,info->id); strcpy(variables->bck.ip,info->ip); strcpy(variables->bck.tcp,info->tcp);
            strcpy(variables->ext.id,info->id); strcpy(variables->ext.ip,info->ip); strcpy(variables->ext.tcp,info->tcp);
            strcpy(variables->id,list[2]); //Guardar o id da minha maquina na struct
            Register(list,*info);
            numero_ancoras = 1;
            primeiro = 1;
        } else {
            strcpy(info->id,list[2]); 
            strcpy(info->net,list[1]);
            SetAncor(*info);
            char boot[30];
            strcat(boot,list[3]); strcat(boot," "); strcat(boot,list[4]); strcat(boot," "); strcat(boot,list[5]);

            if(numero_ancoras == 2){
                strcpy(variables->bck.id,info->id); strcpy(variables->bck.ip,info->ip); strcpy(variables->bck.tcp,info->tcp);
                strcpy(variables->ext.id,list[3]); strcpy(variables->ext.ip,list[4]); strcpy(variables->ext.tcp,list[5]);
                numero_ancoras++;
            }else{
                strcpy(variables->id,list[2]);
                strcpy(variables->ext.id,list[3]); strcpy(variables->ext.ip,list[4]); strcpy(variables->ext.tcp,list[5]); //Guardar informaçoes do externo
            }
            variables->ext.fd = join(list,*variables,*info, boot);
            primeiro = 1;
        }
    }/*
    else if ((strcmp(key, "create") == 0))
    {
    }
    else if ((strcmp(key, "delete") == 0))
    {
    }
    else if (strcmp(key, "get") == 0)
    {
    }*/
    else if ((strcmp(list[0], "show") == 0 && strcmp(list[0], "topology\n") == 0) || strcmp(list[0], "st") == 0){
        PrintContacts(*variables);
    }
    else if (strcmp(list[0], "leave") == 0 && strlen(list[1]) == 0){
        leave(*info,&primeiro);
    }
}

void PrintContacts(Nodes variables){
    printf("Vizinho Externo\n");
    printf("ID:%s IP:%s TCP:%s\n\n",variables.ext.id,variables.ext.ip,variables.ext.tcp);
    printf("Vizinho de Backup\n");
    printf("ID:%s IP:%s TCP:%s\n\n",variables.bck.id,variables.bck.ip,variables.bck.tcp);
    printf("Vizinhos Internos\n");
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
int join(char list[6][50],Nodes variables, Server info, char *selected){
    char str2[128]="REG ";
    int fd;
    char *ext;
    strcat(str2,list[1]);
    strcat(str2," ");
    strcat(str2,list[2]);
    strcat(str2," ");
    strcat(str2,info.ip);
    strcat(str2," ");
    strcat(str2,info.tcp);

    char id[3], ip[16], port[10];
    char *token = strtok(selected,"\n");
    sscanf(token,"%s %s %s",id,ip,port);
    fd = EstablishConnection(ip,port,info); /*IP e PORT sao os parametros do qual eu me quero ligar*/
    ext = SendNew(fd,info); // Manda o NEW e recebe informaçao do backup
    printf("adeus: %s\n",ext);
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

