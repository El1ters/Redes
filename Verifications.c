#include "Main.h"
#include "Verifications.h"

void VerifyIP(int argc,char **argv,Server *info){
    if (argc < 3 || argc > 5){
        printf("Numero invalido de comandos");
        exit(1);
    } else if (strlen(argv[1]) < 7 || strlen(argv[1]) > 15){
        printf("IP inválido");
        exit(1);
    }
    if (strlen(argv[2]) > 5|| ContainLetter(argv[2]) == 1){
        printf("Porta TCP errada");
        exit(1);
    }else
        strcpy(info->tcp, argv[2]);

    char aux1[4],aux2[4],aux3[4],aux4[4];
    int count;
    count = sscanf(argv[1],"%[^.].%[^.].%[^.].%[^.]",aux1,aux2,aux3,aux4);
    if(count < 4){
        printf("IP inválido");
        exit(1);
    }
    if(strlen(aux1) > 3 || strlen(aux2) > 3 || strlen(aux3) > 3 || strlen(aux4) > 3){
        printf("IP inválido");
        exit(1);
    }
    if(ContainLetter(aux1) == 1 || ContainLetter(aux2) == 1 || ContainLetter(aux3) == 1 || ContainLetter(aux4) == 1){
        printf("IP inválido");
        exit(1);
    }  
    strcpy(info->ip,argv[1]); 
}

int ContainLetter(char *argv){
    int i = strlen(argv);
    char *aux = argv;
    for(int j = 0;j != i;j++){
        if(*aux < 48 || *aux > 57)
            return 1;
        aux++;
    }
    return 0;
}

char *verify_id_is_used(char *nodelist){
   char *ids[99], *token;
   int i = 0;
   token = strtok(nodelist,"\n");
   token = strtok(NULL,"\n");
   ids[i] = token;
   i++;
    while(token != NULL){
        token = strtok(NULL,"\n");
        if (token != NULL){
            ids[i] = token;
        }
        i++;
   }
    int line = rand() % (i-1);
    char *selected = (char *) malloc(strlen(ids[line])+1);
    strcpy(selected,ids[line]); 
    return selected; 
}