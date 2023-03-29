#include "Main.h"
#include "Interface.h"

/**********************************************************************************************************************************
 void ConnectTejo(char *string, Server *info,Nodes *variables,int *maxfd)

Esta função recebe um comando do utilizador e atua conforme o comando recebido.

    - Entrada:
        - string: o comando do utilizador;
        - info: um ponteiro para a estrutura Server, que contém as informações do servidor;
        - variables: um ponteiro para a estrutura Nodes, que contém as informações dos nós da rede;
        - maxfd: um ponteiro para um inteiro que contém o número máximo de fds a serem monitorizados.
 
    - Saída:
        - void, logo não retorna nenhum valor
 ***********************************************************************************************************************************/
void ConnectTejo(char *string, Server *info,Nodes *variables,int *maxfd){
    int i = 0;
    static int primeiro = 0; /*0 significa que se pode usar o comando "join", 1 significa o contrario*/
    char list[6][50] = {}, *token;
    token = strtok(string," ");
    /* Obter os argumentos do comando inserido */
    while(token != NULL){
        if(token != NULL)
            strcpy(list[i],token);
        token = strtok(NULL," ");
        i++;
    }
    token = strtok(list[--i],"\n");
    /* 
        * Comando "join" é utilizado para conectar um novo nó à rede.
        * O nó envia uma mensagem UDP para descobrir outros nós na rede.
        * Se for o primeiro servidor na rede, então ele é o primeiro nó e não precisa se conectar a outro.
        * Se já houver outros servidores na rede, ele escolhe um deles para se conectar, aleatoriamente.
    */
    if (strcmp(list[0], "join") == 0 && strlen(list[1]) == 3 && strlen(list[2]) == 2 && strlen(list[3]) == 0){
        char udp_message[10] = "NODES ";
        char compare[20] =  "NODESLIST ";
        // Se for o primeiro servidor na rede, guarda o ID e a rede na struct info e o ID na struct variables
        if(primeiro == 0){
            strcpy(info->id,list[2]); //Guardar o ID do meu server na info
            strcpy(info->net,list[1]); //Guardar o net na info
            strcpy(variables->id,list[2]); //Guardar o ID na struct pedida no projeto
        }
        // Cria a mensagem UDP que será enviada para descobrir outros servidores
        strcat(udp_message,info->net);
        // Cria a string "NODESLIST " com a rede do servidor atual
        strcat(compare,info->net);
        strcat(compare,"\n");
        // Armazena a lista de nós que respondeu à mensagem UDP
        char *nodeslist = Give_List(udp_message,strlen(udp_message));
        /*Verifica se a lista de nos esta vazia*/
        if(strcmp(nodeslist,compare) == 0){ 
            // Se a lista estiver vazia, então este servidor é o primeiro nó
            fflush(stdout);
            strcpy(variables->bck.id,info->id); strcpy(variables->bck.ip,info->ip); strcpy(variables->bck.tcp,info->tcp);
            strcpy(variables->ext.id,info->id); strcpy(variables->ext.ip,info->ip); strcpy(variables->ext.tcp,info->tcp);
            Register(list,*info);
            primeiro = 1;
            first_node = 1; //indica q é o primeiro no entrante no servidor
        } else if(primeiro == 0){
            if(verify_id_is_used(nodeslist,info->id) == 1){
                printf("This ID is already being used\n");
                return;
            }
            char *selected = ChooseID(nodeslist);
            printf("no selecionado: %s\n",selected);
            sscanf(selected,"%s %s %s",variables->ext.id,variables->ext.ip,variables->ext.tcp); //Guardar as informaçoes do externo
            strcpy(variables->bck.id,info->id); strcpy(variables->bck.ip,info->ip); strcpy(variables->bck.tcp,info->tcp);
            variables->ext.fd = join(list,*variables,*info,selected);
            *maxfd = variables->ext.fd;
            FD_SET(variables->ext.fd,&rfds);
            variables->head = insertAtEnd(variables->head,variables->ext.id,variables->ext.id);
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
            strcpy(variables->id,list[2]);
            strcpy(variables->ext.id,list[3]); strcpy(variables->ext.ip,list[4]); strcpy(variables->ext.tcp,list[5]); //Guardar informaçoes do externo
            strcpy(variables->bck.id,info->id); strcpy(variables->bck.ip,info->ip); strcpy(variables->bck.tcp,info->tcp);
            variables->ext.fd = EstablishConnection(list[4],list[5],*info);
            variables->head = insertAtEnd(variables->head,variables->ext.id,variables->ext.id);//Inserir na tabela de expediçao
            SendNew(variables->ext.fd,*info);
            *maxfd = variables->ext.fd;
            FD_SET(variables->ext.fd,&rfds);
            primeiro = 1;
        }
    } else if (strcmp(list[0], "create") == 0){
        if(strlen(list[1])< 101){
            if(add_names(list[1], variables->names) == 1)
                variables->num_names++;
        } else {
            printf("Name escolhido muito longo, inserir outro com até 100 caracteres");
            fflush(stdout);
        }
    }
    else if ((strcmp(list[0], "delete") == 0)){
        if(strlen(list[1])< 101){
            if(clean_names(variables->names, list[1],  variables->num_names ) == 1)
                variables->num_names--;
        }
    }
    else if (strcmp(list[0], "get") == 0){
        Get(list[1],list[2],*variables);
    }else if((strcmp(list[0], "show") == 0 && strcmp(list[1], "routing\n") == 0) || strcmp(list[0], "sr") == 0){
        printList(variables->head);
    }
    else if ((strcmp(list[0], "show") == 0 && strcmp(list[1], "topology\n") == 0) || strcmp(list[0], "st") == 0){
        PrintContacts(*variables);
    }else if((strcmp(list[0],"show") == 0 && strcmp(list[1],"names") == 0) || strcmp(list[0],"sn") == 0){
        printnames(variables->names);
    }
    else if (strcmp(list[0], "leave") == 0 && strlen(list[1]) == 0){
        leave(*info,&primeiro,*variables,&(*maxfd));
    }else if((strcmp(list[0],"clear") == 0 && strcmp(list[1],"routing") == 0) || strcmp(list[0],"cr") == 0){
        freeList(variables->head);
        variables->head = NULL;
    }
}

/********************************************************************************************************************************
    void PrintContacts(Nodes variables)
    
    Função que imprime na tela as informações dos vizinhos de um nó.
    
    - Entrada:
        - variables: variável do tipo Nodes que contém as informações dos vizinhos do nó.
        
    - Saída: 
        - void, logo não retorna nenhum valor
 *********************************************************************************************************************************/
void PrintContacts(Nodes variables) {
    // Imprime as informações do vizinho externo
    printf("Vizinho Externo\n");
    printf("ID:%s IP:%s TCP:%s\n", variables.ext.id, variables.ext.ip, variables.ext.tcp);
    printf("===========================\n");

    // Imprime as informações do vizinho de backup
    printf("Vizinho de Backup\n");
    printf("ID:%s IP:%s TCP:%s\n", variables.bck.id, variables.bck.ip, variables.bck.tcp);
    printf("===========================\n");

    // Percorre o vetor de vizinhos internos e imprime as informações de cada vizinho ativo
    printf("Vizinhos Internos\n");
    for (int i = 0; i != 99; i++) {
        if (variables.intr[i].fd != -1) {
            printf("ID:%s IP:%s TCP:%s\n", variables.intr[i].id, variables.intr[i].ip, variables.intr[i].tcp);
        }  
    }
}
/********************************************************************************************************************************
    void Register(char list[6][50],Server info)
    
    Função que regista um novo nó na rede.
    
    - Entrada:
        - list: vetor de strings que contém as informações sobre o novo nó, como o ID, endereço IP e porta TCP.
        - info: variável do tipo Server que contém as informações do servidor do nó que está a executar a função.
        
    - Saída:
        - void, logo não retorna nenhum valor
 *********************************************************************************************************************************/

void Register(char list[6][50],Server info){
    char str1[10]="NODES 079";
    char str2[128]="REG ";
    // Concatena informações do registo à string str2
    strcat(str2,list[1]);
    strcat(str2," ");
    strcat(str2,list[2]);
    strcat(str2," ");
    strcat(str2,info.ip);
    strcat(str2," ");
    strcat(str2,info.tcp);
    // Envia mensagem com a string str2
    SendMessage(str2,strlen(str2));
}


/********************************************************************************************************************************
int join(char list[6][50], Nodes variables, Server info, char *selected)

Função que estabelece conexão com um nó selecionado e envia uma mensagem REG contendo informações
sobre o novo nó a ser registado na rede.

- Entrada:
    - list: vetor de strings que contém as informações sobre o novo nó como o ID, endereço IP e porta TCP.
    - variables: variável do tipo Nodes que contém informações sobre os nós vizinhos do nó que está a executar a função.
    - info: variável do tipo Server que contém as informações do servidor do nó que está a executar a função.
    - selected: string que contém informações sobre o nó selecionado para se juntar à rede como o ID, endereço IP e porta TCP.
    
- Saída:
    - int: fd da conexão estabelecida com o nó selecionado.
************************************************************************************************************************************/
int join(char list[6][50],Nodes variables, Server info, char *selected){
    char str2[128]="REG ";
    int fd;
    // Constrói a mensagem começada por REG a ser enviada para o novo nó
    strcat(str2,list[1]);
    strcat(str2," ");
    strcat(str2,list[2]);
    strcat(str2," ");
    strcat(str2,info.ip);
    strcat(str2," ");
    strcat(str2,info.tcp);
    // Obtém informações do nó selecionado para se juntar
    char id[3], ip[16], port[10];
    char *token = strtok(selected,"\n");
    sscanf(token,"%s %s %s",id,ip,port);
    // Estabelece conexão com o nó selecionado
    fd = EstablishConnection(ip,port,info); /*IP e PORT sao os parametros do qual eu me quero ligar*/
    SendNew(fd,info); // Manda o NEW e recebe informaçao do backup
    SendMessage(str2,strlen(str2)); // Envia a mensagem REG para o novo nó
    return fd;
}
/********************************************************************************************************************************
void leave(Server info,int *primeiro,Nodes variables,int *maxfd)

Função que remove o nó da rede.

- Entrada:
    - info: variável do tipo Server que contém as informações do servidor do nó que está a ser executado a função.
    - primeiro: ponteiro que indica se o nó é o primeiro da rede (valor 1) ou não (valor 0).
    - variables: variável do tipo Nodes que contém informações sobre o nó e a rede.
    - maxfd: ponteiro para o valor máximo de fd's a ser fiscalizado pela função select.
    
- Saída:
    - void, logo não retorna nenhum valor
*********************************************************************************************************************************/
void leave(Server info,int *primeiro, Nodes variables,int *maxfd){
    //Constrói a mensagem de saída do nó da rede
    char str[128] = "UNREG ";
    strcat(str,info.net); 
    strcat(str," ");
    strcat(str,info.id);
    //verifica se o nó está na rede ou seja se ja foi escrito join
    if(*primeiro == 1){
        SendMessage(str,strlen(str)); // Envia a mensagem de saída para a rede
        (*primeiro) = 0;
    }
    //Liberta a memória alocada para a lista de nós na rede
    freeList(variables.head);             
    //Remove o nó da expedição e fecha a conexão com o nó
    ClearExpedition(variables,variables.id);
    if(variables.ext.fd != -1)
        close(variables.ext.fd);
    //Fecha todas as conexões com os restantes nós
    for(int j = 0;j != 99; j++){
        if(variables.intr[j].fd != -1)
            close(variables.intr[j].fd);
    }
    // Dá reset o valor do maior fd utilizado pelos nós na rede
    *maxfd = 3;
}
/******************************************************************************************************************************
 int add_names(char string[], char array[][100])

Função que adiciona uma nova string a um vetor de strings.

- Entrada:
    - string: Palavra a ser adicionada no vetor.
    - array: Vetor de strings onde a nova string será adicionada.

- Saída:
    - Retorna 1 se a string foi adicionada com sucesso, ou 0 caso contrário.
 ******************************************************************************************************************************/
int add_names(char string[], char array[][100]) {
    // Percorre o vetor de strings procurando a primeira posição vazia
    int i, j;
    for (i = 0; i < 100; i++) {
        if (array[i][0] == '\0') {
            // Copia a string para a posição vazia encontrada
            for (j = 0; j < strlen(string); j++) {
                array[i][j] = string[j];

            }
            // Adiciona o caractere \0 para indicar o fim da string
            array[i][j] = '\0';
            return 1; //  Indica que a string foi adicionada com sucesso
        }
    }
    return 0; // Indica que não foi possível adicionar a string
}

/************************************************************************************************************************************
void printnames(char array[][100])

Função que imprime os nomes armazenados no vetor.

- Entrada:
    - array: matriz de strings que contém os nomes a serem impressos.
    
- Saída:
    - void, logo não retorna nenhum valor
*************************************************************************************************************************************/
void printnames(char array[][100]) {
    int i, j;
    for (i = 0; i < 100; i++) {
        if (array[i][0] == '\0') { // Se a primeira posição da linha atual for '\0', significa que não há mais nomes a serem impressos.
            break; 
        }
        printf("%s\n", array[i]); // Imprime a linha atual
    }
}

/********************************************************************************************************************************
int clean_names(char array[][100], char str[], int contagem)

Função que remove uma string específica do vetor de strings.

- Entrada:
    - array: array de strings a ser manipulado.
    - str: string a ser removida do array.
    - contagem: tamanho do array.

- Saída:
    - Retorna 1 se a string foi removida com sucesso, 0 caso contrário.
********************************************************************************************************************************/
int clean_names(char array[][100], char str[], int contagem){
    int j, i;
    for (j = 0; j < contagem; j++)
    {
        if (strcmp(array[j], str) == 0) // verifica se a string do array é igual à string a ser removida
        {
            for (i = j; j < contagem -1; j++)  // desloca as strings à direita do array para preencher o espaço da string removida
            {
                strcpy(array[j], array[j + 1]);
                if (array[j + 1][0] == '\0')   
                {
                    break; // para quando chegar a uma linha vazia
                }
            }
            array[j][0] = '\0' ; // define a última linha como vazia
            return 1 ; // Se a string foi removida com sucesso
        }
    } return 0; // Se a string não foi encontrada no array
}
/************************************************************************************************************************************
void Get(char *dest,char *text,Nodes variables)

Função que envia uma mensagem de Query para obter informações de um determinado nó .

- Entrada:
    - dest: string que contém o id do nó a que se quer aceder para fazer a verificação.
    - text: string que contém o nome que se pretende verificar a presença.
    - variables: variável do tipo Nodes que contém as informações do nó que está a ser executado na função.
    
- Saída:
    -  void, logo não retorna nenhum valor
*************************************************************************************************************************************/
void Get(char *dest,char *text,Nodes variables){
    // constrói a mensagem a enviar começando com "QUERY "
    char msg[50] = "QUERY ";
    strcat(msg,dest);
    strcat(msg," ");
    strcat(msg,variables.id);
    strcat(msg," ");
    strcat(msg,text);
    strcat(msg,"\n");
    // Envia a mensagem para o nó externo (se existir)
    if(variables.ext.fd != -1)
        write(variables.ext.fd,msg,strlen(msg));
    // Envia a mensagem para todos os nós internos da rede
    for(int i = 0;i != 99;i++){
        if(variables.intr[i].fd != -1)
            write(variables.intr[i].fd,msg,strlen(msg));
    }
}
/************************************************************************************************************************************
void ClearExpedition(Nodes variables, char *node_clear)

Função que envia uma mensagem de WITHDRAW para parar a expedição do nó.

-Entrada:

    -variables: variável do tipo Nodes que contém as informações do nó que está a ser executado na função.
    -node_clear: string que contém o id do nó cuja expedição acaba.
Saída:
    -void, logo não retorna nenhum valor.
*************************************************************************************************************************************/
void ClearExpedition(Nodes variables,char *node_clear){
    char msg[50] = "WITHDRAW ";
    strcat(msg,node_clear);
    strcat(msg,"\n");
    if(variables.ext.fd != -1)
        write(variables.ext.fd,msg,strlen(msg));
    for(int i = 0;i != 99;i++){
        if(variables.intr[i].fd != -1)
            write(variables.intr[i].fd,msg,strlen(msg));
    }
}