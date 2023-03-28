#include "Main.h"
#include "Search.h"

Expedition* insertAtEnd(Expedition* head, char* dest, char* viz){
    Expedition* new_node = (Expedition*) malloc(sizeof(Expedition));

    strcpy(new_node->dest, dest);
    strcpy(new_node->viz, viz);
    new_node->next = NULL;

    if (head == NULL){
        head = new_node;
    }
    else{
        Expedition* current = head;
        if (strcmp(current->dest, dest) == 0 && strcmp(current->viz, viz) == 0){
                free(new_node);  // Free the memory allocated for the new node
                return head;
            }
        while (current->next != NULL){
            if (strcmp(current->next->dest, dest) == 0 && strcmp(current->next->viz, viz) == 0){
                free(new_node);  // Free the memory allocated for the new node
                return head;
            }
            current = current->next;
        }
        current->next = new_node;
    }
    return head;
}

void printList(Expedition* head){
    Expedition* current = head;
    while (current != NULL){
        printf("Destination: %s, Origin: %s\n", current->dest, current->viz);
        current = current->next;
    }
}

void freeList(Expedition* head){
    Expedition* current = head;
    while (current != NULL){
        Expedition* temp = current;
        current = current->next;
        free(temp);
    }
}

void SendQuery(Nodes variables,char *msg, int sock_rec, char *txt, char *dest,char *origin){
    for(int j = 0;j <= variables.num_names; j++){
        if(strcmp(variables.id,dest) == 0){
            if(strcmp(variables.names[j],txt) == 0){
                printf("Encontrou\n");
                BackToSender("CONTENT ",variables,dest,txt,origin);
                return;
            }else{
                BackToSender("NOCONTENT ",variables, dest,txt,origin);
                printf("Nao encontrou\n");
                return;
            }
        }
    }
    if(variables.ext.fd != -1 && variables.ext.fd != sock_rec)
        write(variables.ext.fd,msg,strlen(msg));
    for(int i = 0;i != 99;i++){
        if(variables.intr[i].fd != -1 && variables.intr[i].fd != sock_rec)
            write(variables.intr[i].fd,msg,strlen(msg));
    }
}

void BackToSender(char *string, Nodes variables,char *dest,char *txt,char *origin){
    char aux[30];
    strcpy(aux,string);
    strcat(aux,origin);
    strcat(aux," ");
    strcat(aux,dest);
    strcat(aux," ");
    strcat(aux,txt);
    strcat(aux,"\n");
    char *next = GetNext(variables,dest,origin);
    if(strcmp(variables.ext.id,next) == 0)
        write(variables.ext.fd,aux,strlen(aux));
    else{
        for(int j = 0; j != 99; j++){
            if(strcmp(variables.intr[j].id,next) == 0)
                write(variables.intr[j].fd,aux,strlen(aux));
        }
    }
    free(next);
}

char *GetNext(Nodes variables,char *dest,char *origin){
    char *viz = (char *) malloc(strlen(origin));
    Expedition *atual = variables.head;
    while(atual != NULL){
        if(strcmp(atual->dest, origin)==0){
            strcpy(viz, atual->viz);
            return viz;
        }
        atual = atual->next;
    }
    return NULL;
}

void Withdraw(Nodes variables,int sock_rec,char *msg){
    if(variables.ext.fd != -1 && variables.ext.fd != sock_rec)
        write(variables.ext.fd,msg,strlen(msg));
    for(int i = 0;i != 99;i++){
        if(variables.intr[i].fd != -1 && variables.intr[i].fd != sock_rec)
            write(variables.intr[i].fd,msg,strlen(msg));
    }
}

Expedition *remove_node(Expedition *head, char *no) {
    // Check if list is empty
    if (head == NULL) {
        printf("list is empty\n");
        return NULL;
    }

    Expedition *prev = NULL;
    Expedition *curr = head;
    Expedition *temp = NULL;

    // Loop through the list
    while (curr != NULL) {
        // Check if the current node should be removed
        if (strcmp(curr->dest, no) == 0 || strcmp(curr->viz, no) == 0) {
            // Check if the current node is the head of the list
            if (prev == NULL) {
                head = curr->next;  // Update the head of the list
            } else {
                prev->next = curr->next;     // Update the previous node's next pointer
            }
            temp = curr->next;   // Store the next node before freeing the current node
            free(curr);                      // Free the current node
            curr = temp;                     // Move on to the next node
        } else {
            prev = curr;   // Move to the next node
            curr = curr->next;
        }
    }
    return head;
}
