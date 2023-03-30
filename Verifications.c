#include "Main.h"
#include "Verifications.h"

/********************************************************************************************************************************
void VerifyIP(int argc,char **argv,Server *info)

 Verifica a validade do endereço IP e da porta TCP fornecidos como argumentos.
 
    - Entrada:
        - argc: número de argumentos.
        - argv: vetor de strings com os argumentos
        - info: estrutura Server que será preenchida com o endereço IP e a porta TCP fornecidos.

    - Saída:
        - void, logo não retorna nenhum valor
********************************************************************************************************************************/
void VerifyIP(int argc,char **argv,Server *info){
    // Verifica se o número de argumentos é válido
    if (argc < 3 || argc > 5){
        printf("Numero invalido de comandos");
        exit(1);
    // Verifica se o ip tem tamanho válido
    } else if (strlen(argv[1]) < 7 || strlen(argv[1]) > 15){
        printf("IP inválido");
        exit(1);
    }
    // Verifica se a porta TCP é válida
    if (strlen(argv[2]) > 5|| ContainLetter(argv[2]) == 1){
        printf("Porta TCP errada");
        exit(1);
    }else
        strcpy(info->tcp, argv[2]);
    // Divide o endereço IP em 4 partes e verifica se cada parte é válida
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
    // Armazena o endereço IP e a porta TCP fornecidos na estrutura Server
    strcpy(info->ip,argv[1]); 
    strcpy(info->tcp,argv[2]);
    if(argc > 3 && strlen(argv[3]) != 0)
        strcpy(regIP,argv[3]);
    if(argc > 4 && strlen(argv[4]) != 0)    
        strcpy(regUDP,argv[4]);
}

/********************************************************************************************************************************
int ContainLetter(char *argv)

Função que verifica se uma string contém letras ou não.
 
    - Entrada:
        - argv: string a vrificar

    - Saída:
        - 1 se a string contém letras, 0 caso contrário
********************************************************************************************************************************/
int ContainLetter(char *argv){
    int i = strlen(argv);
    char *aux = argv;
    for(int j = 0;j != i;j++){
        if(*aux < 48 || *aux > 57)
            return 1; // 1 caso contiver letras
        aux++;
    }
    return 0; // 0 caso nao contiver
}

/********************************************************************************************************************************
char *ChooseID(char *nodelist)

Escolhe um ID aleatório de um nó na lista de nós.
 
    - Entrada:
        - nodelist: Lista de nós

    - Saída:
        - Retorna o ID selecionado
********************************************************************************************************************************/
char *ChooseID(char *nodelist){
   char *ids[99], *token;
   int i = 0;
   // Extrai o início da lista e descarta-o, já que é apenas um cabeçalho.
   token = strtok(nodelist,"\n");
   token = strtok(NULL,"\n");
   // Armazena o primeiro ID no vetor.
   ids[i] = token;
   i++;
   //Extrai os restantes e adiciona também no vetor
    while(token != NULL){
        token = strtok(NULL,"\n");
        if (token != NULL){
            ids[i] = token;
        }
        i++;
   }
   // Escolhe um id aleatório no vetor de IDs.
    int line = rand() % (i-1);
    // Limpa o buffer de saída para garantir que o texto será impresso imediatamente.
    fflush(stdout);
    // Aloca dinamicamente memória para armazenar o ID selecionado.
    char *selected = (char *) malloc(strlen(ids[line])+1);
    // Copia o ID selecionado para a memória.
    strcpy(selected,ids[line]); 
    // Retorna o ID selecionado.
    return selected; 
}

/********************************************************************************************************************************
int verify_id_is_used(char *nodeslist,char *ID)

Verifica se um ID já está a ser usado na lista de nós.
 
    - Entrada:
        - nodelist: Lista de nós
        - id: id a ser verificado

    - Saída:
        - Retorna 1 se o ID já está sendo usado ou 0 caso contrário.
********************************************************************************************************************************/
int verify_id_is_used(char *nodeslist,char *ID){
    char *token, aux[3];
    char *list = (char *)malloc(1800);
    // copia a lista de nós para a variável auxiliar
    strcpy(list,nodeslist);
    // obtém a segunda linha da lista de nós, após descartar a primeira
    token = strtok(list,"\n");
    token = strtok(NULL,"\n");
    sscanf(token,"%s",aux); // lê o primeiro token da segunda linha (o ID do primeiro nó)
    if(strcmp(aux,ID) == 0){
        free(list);
        // compara o ID com o ID fornecido
        return 1; //se já está a ser usado
    } 
        
    while(token != NULL){ // percorre todas as linhas da lista de nós
        token = strtok(NULL,"\n"); //próxima linha
        if(token != NULL){
            sscanf(token,"%s",aux);
            if(strcmp(aux,ID) == 0){
                free(list);
                return 1;
            }
                
        }  
    }
    free(list);
    return 0; // retorna 0 se o ID não está sendo usado
}