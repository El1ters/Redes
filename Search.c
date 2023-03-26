#include "Main.h"
#include "Search.h"

Expedition* insertAtEnd(Expedition* head, char* dest, char* viz){
    Expedition* new_node = (Expedition*) malloc(sizeof(Expedition));

    strcpy(new_node->dest, dest);
    strcpy(new_node->viz, viz);
    new_node->next = NULL;

    if (head == NULL) {
        head = new_node;
    }
    else {
        Expedition* current = head;
        while (current->next != NULL) {
            if (strcmp(current->next->dest, dest) == 0 && strcmp(current->next->viz, viz) == 0) {
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
    while (current != NULL) {
        printf("Destination: %s, Origin: %s\n", current->dest, current->viz);
        current = current->next;
    }
}

void freeList(Expedition* head){
    Expedition* current = head;
    while (current != NULL) {
        Expedition* temp = current;
        current = current->next;
        free(temp);
    }
}