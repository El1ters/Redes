#include "Main.h"

int first_node = 0;
fd_set rfds;

/**************************************************************************************************************************************
int main(int argc, char **argv)

Função principal do servidor
    - Entradas:
        -argc Número de argumentos passados na linha de comando
        -argv Vetor de strings contendo os argumentos passados na linha de comando
    -Saída 
        -Retorna 0 se o programa terminar sem erros
***************************************************************************************************************************************/
int main(int argc, char **argv)
{
    srand(time(NULL)); // inicializa o gerador de números aleatórios
    int fd, newfd;
    int maxfd, counter;
    char string[128];
    socklen_t addrlen;
    struct sockaddr_in addr;
    addrlen = sizeof(addr);
    Server info; // informações do servidor
    Nodes variables; // variáveis do servidor
    variables.num_names = 0; //inicializa o número de nomes a zero
    variables.head = NULL;
    memset(&variables.names,0,sizeof(variables.names));
    int number_on = 0; // inicializa o número de clientes como zero
    for (int k = 0; k != 99; k++)
    {
        variables.intr[k].fd = -1; // inicializa os fds dos internos como -1
    }
    variables.ext.fd = -1; // inicializa os fds do externo como -1
    VerifyIP(argc, argv, &info); // info->ip e info->tcp fica guardado
    fd = Init_Server(info); // inicializa o servidor com as informações de IP e porta
    maxfd = fd; // define o valor máximo de fd como o fd atual
    FD_ZERO(&rfds); // inicializa o conjunto de fds lidos como vazio
    while (1)
    {
        // Define os fds a serem monitorizados na entrada padrão e no fd.
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(fd, &rfds);
         // Percorre as estruturas de dados de todos os clientes conectados e adiciona os fds a rfds.
        for (int j = 0; j != 99; j++)
        {
            if (variables.intr[j].fd != -1)
                FD_SET(variables.intr[j].fd, &rfds);
        }
        // Adiciona o fd da conexão externa (se existir) a rfds.
        FD_SET(variables.ext.fd, &rfds);
        // Espera até que algum fd esteja pronto para leitura.
        // O valor de retorno indica quantos fds estão com algo para ler.
        counter = select(maxfd + 1, &rfds, (fd_set *)NULL, (fd_set *)NULL, (struct timeval *)NULL);
        // Verifica se há entrada de dados na entrada padrão.
        if (FD_ISSET(STDIN_FILENO, &rfds))
        {
            // Limpa o buffer de string.
            bzero(string, 128);
            // Verifica se a entrada foi "exit".
            fgets(string, 128, stdin);
            if (strcmp(string, "exit\n") == 0)
            {
                // Fecha o fd e encerra o programa.
                close(fd);
                exit(0);
            }
            // Verifica se a entrada foi uma linha vazia.
            if (strcmp(string, "\n") == 0)
                continue;
            //Trata a entrada como comando do jogo e envia para os clientes conectados.
            ConnectTejo(string, &info, &variables, &maxfd);
        }
        // Verifica se há conexão de entrada no fd.
        if (FD_ISSET(fd, &rfds))
        {
            // Aceita a conexão de entrada e obtém um novo fd para a conexão.
            if ((newfd = accept(fd, (struct sockaddr *)&addr, (socklen_t *)&addrlen)) == -1)
                exit(1);
            // Atualiza o valor máximo de fd.
            if (newfd > maxfd)
                maxfd = newfd;

            if (first_node == 1)
            {
                // Armazena o fd da conexão externa na estrutura variables.
                variables.ext.fd = newfd;
                // Incrementa o contador first_node conexão já foi aceite.
                first_node++;
                // Adiciona o fd da conexão externa a rfds.
                FD_SET(variables.ext.fd, &rfds);
            }
            else
            {
                // Percorre as estruturas de dados de todos os clientes conectados e adiciona o novo fd à primeira posição vaga.
                for (int k = 0; k != 99; k++)
                {
                    if (variables.intr[k].fd == -1)
                    {
                        // Armazena o fd da conexão interna na estrutura variables.
                        variables.intr[k].fd = newfd;
                        // Adiciona o fd da conexão interna a rfds.
                        FD_SET(variables.intr[k].fd, &rfds);
                        break;
                    }
                }
                number_on++;
            }
        }
        // Percorre todos os clientes conectados
        for (int j = 0; j != number_on; j++){
            // Verifica se há dados prontos para serem lidos em cada um dos clientes conectados
            if (FD_ISSET(variables.intr[j].fd, &rfds)){
                int n;
                char buffer[128] = {}; // Buffer para armazenar os dados recebidos
                char message[4][20] = {}; // Vetor para armazenar as diferentes partes da mensagem recebida
                n = read(variables.intr[j].fd, buffer, 128); // Lê os dados do socket e armazena no buffer
                if (n == 0){ // Caso nenhum dado tenha sido recebido
                    printf("Perdeu conexao com o interno\n");
                    ClearExpedition(variables,variables.intr[j].id); // Remove o cliente da tabela de expedição
                    variables.head = remove_node(variables.head,variables.intr[j].id); // Remove o cliente da lista encadeada
                    close(variables.intr[j].fd); // Fecha a conexão com o cliente
                    FD_CLR(variables.intr[j].fd, &rfds); // Remove o socket do conjunto de sockets prontos para leitura
                    memset(&variables.intr[j], 0, sizeof(variables.intr[j])); // Limpa a estrutura do cliente
                    variables.intr[j].fd = -1; // Define o fd como inválido
                    break;
                }
                else{
                    sscanf(buffer, "%s %s %s %s", message[0], message[1], message[2], message[3]);
                    write(1, "received: ", 11); // Imprime que uma mensagem foi recebida
                    write(1, buffer, strlen(buffer) + 1); // Imprime o conteúdo da mensagem recebida
                    if (strcmp(message[0], "NEW") == 0){
                        strcpy(variables.intr[j].id, message[1]); //Armazena o id do cliente
                        strcpy(variables.intr[j].ip, message[2]); //Armazena o ip do cliente
                        strcpy(variables.intr[j].tcp, message[3]); //Armazena a porta do cliente
                        variables.head = insertAtEnd(variables.head,variables.intr[j].id,variables.intr[j].id);//Adicionar novo nó na tabela de expediçao
                        SendExtern(variables.intr[j].fd, variables); // Envia uma mensagem para os clientes externos informando sobre o novo cliente conectado
                    }else if(strcmp(message[0], "QUERY") == 0){
                        variables.head = insertAtEnd(variables.head,message[2],variables.intr[j].id); // Adiciona o cliente à tabela de expedição
                        SendQuery(variables, buffer,variables.intr[j].fd,message[3],message[1],message[2]); // Encaminha a mensagem para os clientes
                    }else if(strcmp(message[0], "CONTENT") == 0 || strcmp(message[0], "NOCONTENT") == 0){
                        strcat(message[0]," ");
                        if(strcmp(variables.id,message[1]) != 0)
                            BackToSender(message[0],variables,message[2],message[3],message[1]); //Enviaa mensagem de volta ao remetente original
                    }else if(strcmp(message[0], "WITHDRAW") == 0){
                        variables.head = remove_node(variables.head,message[1]); //Remove o nó com o ID da lista
                        Withdraw(variables,variables.intr[j].fd, buffer); //Envia a mensagem "WITHDRAW" para todos os nós
                    }
                }
            }
        }

        if (FD_ISSET(variables.ext.fd, &rfds)){
            int n;
            char buffer[128] = {};
            char message[4][20] = {};
            n = read(variables.ext.fd, buffer, 128);

            if (n == 0){ // caso não haja nada para ler do socket
                ClearExpedition(variables,variables.ext.id); // Limpa as informações de expedição associadas a este socket
                variables.head = remove_node(variables.head,variables.ext.id); //Remove o nó correspondente à conexão do socket da lista de nós
                HandleNode(&variables,&maxfd,info);
            }
            else{
                sscanf(buffer, "%s %s %s %s", message[0], message[1], message[2], message[3]); // Lê a mensagem do buffer e a armazena em um array de strings
                // Imprime a mensagem recebida
                write(1, "received: ", 11); 
                write(1, buffer, strlen(buffer) + 1);
                if (strcmp(message[0], "NEW") == 0){
                    strcpy(variables.ext.id, message[1]); // armazena o ID do novo nó externo
                    strcpy(variables.ext.ip, message[2]); // armazena o IP do novo nó externo
                    strcpy(variables.ext.tcp, message[3]); // armazena a porta do novo nó externo

                    variables.head = insertAtEnd(variables.head,variables.ext.id,variables.ext.id);//Adicionar o novo nó na tabela de expediçao
                    SendExtern(variables.ext.fd, variables); // envia a informação do novo nó para os nós
                }
                else if (strcmp(message[0], "EXTERN") == 0){
                    strcpy(variables.bck.id, message[1]); // armazena o ID do backup
                    strcpy(variables.bck.ip, message[2]); // armazena o IP do backup
                    strcpy(variables.bck.tcp, message[3]); // armazena a porta do backup
                    if(strcmp(message[1],variables.id) != 0) //Para nao ficar com as informaçoes dele proprio
                        variables.head = insertAtEnd(variables.head,variables.bck.id,variables.ext.id);
                }else if(strcmp(message[0], "QUERY") == 0){
                    variables.head = insertAtEnd(variables.head,message[2],variables.ext.id);
                    SendQuery(variables, buffer,variables.ext.fd,message[3],message[1],message[2]); // envia a mensagem QUERY para todos os nós conhecidos até encontrar o desejado, exceto o que a enviou 
                }else if(strcmp(message[0], "CONTENT") == 0 || strcmp(message[0], "NOCONTENT") == 0){
                    strcat(message[0]," "); // Concatena um espaço após a palavra "CONTENT" ou "NOCONTENT" no vetor
                    // Verifica se a mensagem não é destinada a este cliente
                    if(strcmp(variables.id,message[1]) != 0)
                        BackToSender(message[0],variables,message[2],message[3],message[1]); // Envia a mensagem de volta ao remetente
                }else if(strcmp(message[0], "WITHDRAW") == 0){
                    variables.head = remove_node(variables.head,message[1]); // Remove um nó da lista de usuários conectados
                    Withdraw(variables,variables.ext.fd,buffer); // Envia uma mensagem de confirmação de saída
                }
            }
        }
    }
    return 0;
}