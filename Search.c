#include "Main.h"
#include "Search.h"

/********************************************************************************************************************************
Expedition* insertAtEnd(Expedition* head, char* dest, char* viz)

Insere um novo nó no final de uma lista encadeada de expeditions

- Entrada:
    - head: ponteiro para o primeiro nó da lista.
    - dest: string com o destino da expedição a ser inserida.
    - viz : string com a vizinhança da expedição a ser inserida.

- Saída:
    - Retorna ponteiro para o primeiro nó da lista atualizada
********************************************************************************************************************************/
Expedition* insertAtEnd(Expedition* head, char* dest, char* viz){
    // Aloca espaço para o novo nó e define os valores
    Expedition* new_node = (Expedition*) malloc(sizeof(Expedition));

    strcpy(new_node->dest, dest);
    strcpy(new_node->viz, viz);
    new_node->next = NULL;
    // Verifica se a lista está vazia e, se sim, define o novo nó como o primeiro
    if (head == NULL){
        head = new_node;
    }
    else{
        // Percorre a lista até ao último nó
        Expedition* current = head;
        if (strcmp(current->dest, dest) == 0 && strcmp(current->viz, viz) == 0){
            // Quando encontra um nó com as mesmas informações, liberta a memória alocada para o novo nó e retorna o ponteiro para o primeiro nó
                free(new_node);  
                return head;
            }
        while (current->next != NULL){
            if (strcmp(current->next->dest, dest) == 0 && strcmp(current->next->viz, viz) == 0){
                 // Se encontram um nó com as mesmas informações, liberta a memória alocada para o novo nó e retorna o ponteiro para o primeiro nó
                free(new_node);  
                return head;
            }
            current = current->next;
        }
        // Define o próximo nó do último nó como o novo nó
        current->next = new_node;
    }
    // Retorna o ponteiro para o primeiro nó da lista atualizada
    return head;
}

/********************************************************************************************************************************
void printList(Expedition* head)

 Imprime os destinos e vizinhos de uma lista.
 
 - Entrada:
    - head: ponteiro para o primeiro destino em análise da lista.

- Saída:
     - void, logo não retorna nenhum valor
********************************************************************************************************************************/
void printList(Expedition* head){
    Expedition* current = head;
    while (current != NULL){
        printf("Destination: %s, Origin: %s\n", current->dest, current->viz); //Dá print do destino e do vizinho
        current = current->next;
    }
}

/********************************************************************************************************************************
void freeList(Expedition* head)

 Liberta a memória alocada para todos os destinos em análise de uma lista.
 
 - Entrada:
    - head: ponteiro para o primeiro destino em análise da lista.

- Saída:
     - void, logo não retorna nenhum valor
********************************************************************************************************************************/
void freeList(Expedition* head){
    Expedition* current = head;
    while (current != NULL){
        Expedition* temp = current; // Armazena o ponteiro para o destino em análise na variável temporária
        current = current->next; // Avança o ponteiro para o próximo destino
        free(temp);   // Liberta a memória alocada para o destino.
    }
}
/********************************************************************************************************************************
void SendQuery(Nodes variables,char *msg, int sock_rec, char *txt, char *dest,char *origin)

 Função responsável por enviar um query a um nó da rede

    - Entrada:
        - variables: estrutura com informações sobre a rede de comunicação e nós
        - msg: mensagem a ser enviada
        - sock_rec: identificador do socket do nó que recebeu a mensagem
        - txt: texto que o querry tem
        - dest: nó de destino do query
        - origin: nó de origem do query

- Saída:
     - void, logo não retorna nenhum valor
********************************************************************************************************************************/
void SendQuery(Nodes variables,char *msg, int sock_rec, char *txt, char *dest,char *origin){
    // Verifica se o conteúdo procurado existe no nó destino
    if(strcmp(variables.id,dest) == 0){
        for(int j = 0;j <= variables.num_names; j++){
            if(strcmp(variables.names[j],txt) == 0){
                BackToSender("CONTENT ",variables,dest,txt,origin); //Envia content caso encontre no nó de destino
                return;
            }
        }
        BackToSender("NOCONTENT ",variables, dest,txt,origin); //Envia nocontent caso não encontre no nó de destino
        return;
    }
    // Verifica se é possível aceder diretamente ao nó destino
    if(AccessDirectly(variables,dest,origin,msg) == 1){
        return;
    }
    // Encaminha a mensagem para o nó externo, caso exista
    if(variables.ext.fd != -1 && variables.ext.fd != sock_rec)
        write(variables.ext.fd,msg,strlen(msg));
    // Encaminha a mensagem para os nós internos, caso existam
    for(int i = 0;i != 99;i++){
        if(variables.intr[i].fd != -1 && variables.intr[i].fd != sock_rec)
            write(variables.intr[i].fd,msg,strlen(msg));
    }
}

/********************************************************************************************************************************
void BackToSender(char *string, Nodes variables,char *dest,char *txt,char *origin)

Envia uma mensagem de volta para o remetente original ou para o próximo nó em uma rede de comunicação
 
    - Entrada:
        -string - mensagem a ser enviada
        -variables - estrutura com informações sobre a rede de comunicação e nós
        -dest - destino da mensagem
        -txt - conteúdo da mensagem
        -origin - remetente original da mensagem

    - Saída:
        - void, logo não retorna nenhum valor
********************************************************************************************************************************/
void BackToSender(char *string, Nodes variables,char *dest,char *txt,char *origin){
    char aux[30] = {};
    strcpy(aux,string); // Copia a mensagem para uma variável auxiliar
    strcat(aux,origin);
    strcat(aux," ");
    strcat(aux,dest);
    strcat(aux," ");
    strcat(aux,txt);
    strcat(aux,"\n");
    // Obtém o próximo nó na rota para o destino
    char *next = GetNext(variables,dest,origin);
    // Se o próximo nó for o nó externo, envia a mensagem de volta para o remetente original
    if(variables.ext.fd != -1 && strcmp(variables.ext.id,next) == 0)
        write(variables.ext.fd,aux,strlen(aux));
    else{
        for(int j = 0; j != 99; j++){
            // Se o próximo nó for um nó interno, envia a mensagem para o próximo nó na rota
            if(variables.intr[j].fd != -1 && strcmp(variables.intr[j].id,next) == 0)
                write(variables.intr[j].fd,aux,strlen(aux));
        }
    }
    free(next); // Liberta a memória alocada para a string "next"
}

/********************************************************************************************************************************
char *GetNext(Nodes variables,char *dest,char *origin)

Retorna o identificador do próximo vizinho de um determinado nó de origem na lista.
 
    - Entrada:
        -variables - estrutura com informações sobre a rede de comunicação e nós
        -dest - destino da mensagem
        -origin - remetente original da mensagem

    - Saída:
        - char* ponteiro para uma string com o identificador do próximo vizinho, ou NULL se não houver próximo vizinho
********************************************************************************************************************************/
char *GetNext(Nodes variables,char *dest,char *origin){
    char *viz = (char *) malloc(strlen(origin) + 1); // aloca memória para a string que irá armazenar o identificador do próximo vizinho
    Expedition *atual = variables.head;  // define o ponteiro atual como o primeiro linha a visitar da lista.
    while(atual != NULL){
        if(strcmp(atual->dest, origin) == 0){ //verificar se encontra em alguma linha da lista o origin pretendido se sim copia para o viz o correspondente vizinho da lista
            strcpy(viz, atual->viz);
            return viz;
        }
        atual = atual->next;  // avança para o próxima linha da lista
    }
    return NULL; // retorna NULL se não houver próximo vizinho
}

/********************************************************************************************************************************
void Withdraw(Nodes variables,int sock_rec,char *msg)

Esta função envia uma mensagem para todos os clientes conectados, exceto aquele a que já enviou.
 
    - Entrada:
        -variables - estrutura com informações sobre a rede de comunicação e nós
        -sock_rec - socket do cliente que enviou a mensagem
        -msg - mensagem a enviar

    - Saída:
        - void, logo não retorna nenhum valor
********************************************************************************************************************************/
void Withdraw(Nodes variables,int sock_rec,char *msg){
    if(variables.ext.fd != -1 && variables.ext.fd != sock_rec)
        write(variables.ext.fd,msg,strlen(msg));
    for(int i = 0;i != 99;i++){
        if(variables.intr[i].fd != -1 && variables.intr[i].fd != sock_rec)
            write(variables.intr[i].fd,msg,strlen(msg));
    }
}

/********************************************************************************************************************************
Expedition *remove_node(Expedition *head, char *no)

Remove uma linha da lista que contenha uma determinada string no campo "dest" ou "viz"
    - Entrada:
        - head: ponteiro para o primeiro nó da lista.
        - no: string a procurar.

    - Saída:
        - Retorna ponteiro para o primeiro nó da lista atualizada
********************************************************************************************************************************/
Expedition *remove_node(Expedition *head, char *no) {
    // Verifica se a lista está vazia
    if (head == NULL) {
        printf("list is empty\n");
        return NULL;
    }

    Expedition *prev = NULL;
    Expedition *curr = head;
    Expedition *temp = NULL;

    // Percorre a lista 
    while (curr != NULL) {
        // Verifica se a linha atual deve ser removido
        if (strcmp(curr->dest, no) == 0 || strcmp(curr->viz, no) == 0) {
            // Verifica se a linha atual é o primeiro da lista
            if (prev == NULL) {
                head = curr->next;  // Atualiza o ponteiro para a linha atual da lista
            } else {
                prev->next = curr->next; // Atualiza o ponteiro "next" da linha anterior
            }
            temp = curr->next;   // Armazena o próxima linha antes de libertar a memória da atual
            free(curr);                      // Liberta a memória da linha atual
            curr = temp;                     //Move para a próxima linha
        } else {
            prev = curr;  //Move para a próxima linha
            curr = curr->next;
        }
    }
    return head;
}

/********************************************************************************************************************************
int AccessDirectly(Nodes variables,char *dest,char *origin, char *msg)

Envia mensagem diretamente para o nó de destino caso já conheça o caminho.

    - Entrada:
        - variables: estrutura com informações sobre a rede de comunicação e nós
        - dest: nome do nó de destino da mensagem
        - origin: nome do nó de origem da mensagem
        - msg: mensagem a ser enviada para o nó de destino

    - Saída:
        - Retorna 1 se a mensagem foi enviada diretamente para o nó de destino ou 0 caso contrário
********************************************************************************************************************************/
int AccessDirectly(Nodes variables,char *dest,char *origin, char *msg){
    Expedition *to_compare = variables.head;
    while(to_compare != NULL){
        //ver se o nó a que se quer chegar está na parte dos destinos
        if(strcmp(to_compare->dest, dest) == 0){
            //Ver se a ligação existe e ver se tenho de mandar query ao externo
            if(variables.ext.fd != -1 && strcmp(variables.ext.id,to_compare->viz) == 0){
                write(variables.ext.fd,msg,strlen(msg));
                return 1;
            }
            //fazer um for pelos meus internos e ver se a ligaçao existe e ver se tenho q mandar query ao interno
            for(int j = 0;j != 99;j++){
                if(variables.intr[j].fd != -1 && strcmp(variables.intr[j].id,to_compare->viz) == 0){
                    write(variables.intr[j].fd,msg,strlen(msg));
                    return 1;
                }
            }    
        }
        to_compare = to_compare->next;
    }
    return 0;
}