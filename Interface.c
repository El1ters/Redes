#include "Main.h"
#include "Interface.h"

void ConnectTejo(char *string, Server *info,Nodes *variables,int *maxfd){
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

        /*Verifica se a lista de nos esta vazia*/
        if(strcmp(nodeslist,compare) == 0){ 
            strcpy(variables->bck.id,info->id); strcpy(variables->bck.ip,info->ip); strcpy(variables->bck.tcp,info->tcp);
            strcpy(variables->ext.id,info->id); strcpy(variables->ext.ip,info->ip); strcpy(variables->ext.tcp,info->tcp);
            Register(list,*info);
            primeiro = 1;
            first_node = 1; //indica q é o primeiro no entrante no servidor
        } else if(primeiro == 0){
            char *selected = verify_id_is_used(nodeslist);
            printf("no selecionado: %s\n",selected);
            sscanf(selected,"%s %s %s",variables->ext.id,variables->ext.ip,variables->ext.tcp); //Guardar as informaçoes do externo
            strcpy(variables->bck.id,info->id); strcpy(variables->bck.ip,info->ip); strcpy(variables->bck.tcp,info->tcp);
            variables->ext.fd = join(list,*variables,*info,selected);
            *maxfd = variables->ext.fd;
            FD_SET(variables->ext.fd,&rfds);
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
            primeiro = 1;
            first_node = 1;
        } else {
            strcpy(info->id,list[2]); 
            strcpy(info->net,list[1]);
            char boot[30];
            strcat(boot,list[3]); strcat(boot," "); strcat(boot,list[4]); strcat(boot," "); strcat(boot,list[5]);
            strcpy(variables->ext.id,list[3]); strcpy(variables->ext.ip,list[4]); strcpy(variables->ext.tcp,list[5]); //Guardar informaçoes do externo
            strcpy(variables->bck.id,info->id); strcpy(variables->bck.ip,info->ip); strcpy(variables->bck.tcp,info->tcp);
            variables->ext.fd = EstablishConnection(list[4],list[5],*info);
            SendNew(variables->ext.fd,*info);
            *maxfd = variables->ext.fd;
            FD_SET(variables->ext.fd,&rfds);
            primeiro = 1;
        }
    } else if (strcmp(list[0], "create") == 0){
        if(strlen(list[1])< 101){
            if(add_names(list[1], variables->names)==1)
                variables->num_names += 1;
            printnames(variables->names);
        } else {
            printf("Name escolhido muito longo, inserir outro com até 100 caracteres");
            fflush(stdout);
        }
    }
    else if ((strcmp(list[0], "delete") == 0)){
        if(strlen(list[1])< 101){
            if(clean_names(variables->names, list[1],  variables->num_names )==1)
                variables->num_names -= 1;
            printnames(variables->names);
        }
    }
//     else if (strcmp(list[0], "get") == 0)
//     { if(strlen(list[1])<3 )// acho que deviamos acrescentar condicao de ver se o id que procuramos existe na nodeslist mas nao sei se o verify_if_id is used ta a dar isso)
//    {
    // char serching_node[3];
    // strcp(vari, list[1]);
//       if(strlen(list[2])<101){
//         char name_searched[101];
//         strcpy(name_searched,list[2]);
//           for (i = 0;i< variables->num_names;i++)  {
//             if (strcmp(variables->names[i][atoi(variables->id)], name_searched) == 0)  
//             printf("Name:%s encontrado no nó %s", name_searched, variables->id );
//       } 
//       }else printf("Não há nomes com mais de 100 caracteres");
//    }else printf("O nó tem de ter um valor de 1 a 99, se escolheu um valor entre 1 a 99 e falhou é porque esse nó não se encontra na rede");
//     }
    else if ((strcmp(list[0], "show") == 0 && strcmp(list[1], "topology\n") == 0) || strcmp(list[0], "st") == 0){
        PrintContacts(*variables);
    }
    else if (strcmp(list[0], "leave") == 0 && strlen(list[1]) == 0){
        leave(*info,&primeiro,*variables);
    }
}

void PrintContacts(Nodes variables){
    printf("Vizinho Externo\n");
    printf("ID:%s IP:%s TCP:%s\n",variables.ext.id,variables.ext.ip,variables.ext.tcp);
    printf("========================\n");
    printf("Vizinho de Backup\n");
    printf("ID:%s IP:%s TCP:%s\n",variables.bck.id,variables.bck.ip,variables.bck.tcp);
    printf("========================\n");
    printf("Vizinhos Internos\n");
    for(int i = 0;i != 99;i++)
        if(variables.intr[i].fd != -1)
            printf("ID:%s IP:%s TCP:%s\n",variables.intr[i].id,variables.intr[i].ip,variables.intr[i].tcp);
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
    SendNew(fd,info); // Manda o NEW e recebe informaçao do backup
    SendMessage(str2,strlen(str2));
    return fd;
}

void leave(Server info,int *primeiro,Nodes variables){
    char str[128] = "UNREG ";
    strcat(str,info.net); 
    strcat(str," ");
    strcat(str,info.id);
    if(*primeiro == 1){
        SendMessage(str,strlen(str));
        (*primeiro) = 0;
    }
    if(strcmp(variables.bck.id,variables.id) == 0){
        close(variables.ext.fd);
        for(int j = 0;j != 99; j++){
            if(variables.intr[j].fd != -1)
                close(variables.intr[j].fd);
        }
    }else{
        close(variables.ext.fd);
        for(int j = 0;j != 99; j++){
            if(variables.intr[j].fd != -1)
                close(variables.intr[j].fd);
        }
    }
}

int add_names(char string[], char array[][100]) {
    int i, j;
    for (i = 0; i < 100; i++) {
        if (array[i][0] == '\0') {
            for (j = 0; j < strlen(string); j++) {
                array[i][j] = string[j];

            }
            array[i][j] = '\0';
            return 1;
        }
    }
    return 0;
}

void printnames(char array[][100]) {
    int i, j;
    for (i = 0; i < 100; i++) {
        if (array[i][0] == '\0') {
            break; 
        }
        printf("%s\n", array[i]); // print the current row
    }
}

int clean_names(char array[][100], char str[], int contagem){
    int j, i;
    for (j = 0; j < contagem; j++)
    {
        if (strcmp(array[j], str) == 0)
        {
            for (i = j; j < contagem -1; j++)
            {
                strcpy(array[j], array[j + 1]);
                if (array[j + 1][0] == '\0')
                {
                    break; // stop shifting if we reach an empty row
                }
            }
            // variables.num_names--;
            array[j][0] = '\0' ;
            return 1 ;
        }
    } return 0;
}