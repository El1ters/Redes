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

void SendQuery(Nodes variables,char *msg, int sock_rec, char *txt, char *dest){
    for(int j = 0;j <= variables.num_names; j++){
        if(strcmp(variables.id,dest) == 0){
            if(strcmp(variables.names[j],txt) == 0){
                printf("Encontrou\n");
                return;
            }else{
                printf("Nao encontrou\n");
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

void BackToSender(){
    
}
