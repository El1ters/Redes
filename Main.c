#include "Main.h"

int first_node = 0;
fd_set rfds;

int main(int argc, char **argv)
{
    srand(time(NULL));
    int fd, newfd;
    int maxfd, counter;
    char string[128];
    socklen_t addrlen;
    struct sockaddr_in addr;
    addrlen = sizeof(addr);
    Server info;
    Nodes variables;
    variables.num_names = 0;
    variables.head = NULL;
    int number_on = 0;
    for (int k = 0; k != 99; k++)
    {
        variables.intr[k].fd = -1;
    }
    variables.ext.fd = -1;
    VerifyIP(argc, argv, &info); // info->ip e info->tcp fica guardado
    FD_ZERO(&rfds);
    fd = Init_Server(info);
    maxfd = fd;
    while (1)
    {
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(fd, &rfds);
        for (int j = 0; j != 99; j++)
        {
            if (variables.intr[j].fd != -1)
                FD_SET(variables.intr[j].fd, &rfds);
        }
        FD_SET(variables.ext.fd, &rfds);
        counter = select(maxfd + 1, &rfds, (fd_set *)NULL, (fd_set *)NULL, (struct timeval *)NULL);
        if (FD_ISSET(STDIN_FILENO, &rfds))
        {
            bzero(string, 128);
            fgets(string, 128, stdin);
            if (strcmp(string, "exit\n") == 0)
            {
                close(fd);
                exit(0);
            }
            if (strcmp(string, "\n") == 0)
                continue;
            ConnectTejo(string, &info, &variables, &maxfd);
        }

        if (FD_ISSET(fd, &rfds))
        {
            if ((newfd = accept(fd, (struct sockaddr *)&addr, (socklen_t *)&addrlen)) == -1)
                exit(1);

            if (newfd > maxfd)
                maxfd = newfd;

            if (first_node == 1)
            {
                variables.ext.fd = newfd;
                first_node++;
                FD_SET(variables.ext.fd, &rfds);
            }
            else
            {
                for (int k = 0; k != 99; k++)
                {
                    if (variables.intr[k].fd == -1)
                    {
                        variables.intr[k].fd = newfd;
                        FD_SET(variables.intr[k].fd, &rfds);
                        break;
                    }
                }
                number_on++;
            }
        }

        for (int j = 0; j != number_on; j++)
        {
            if (FD_ISSET(variables.intr[j].fd, &rfds))
            {
                int n;
                char buffer[128] = {};
                char message[4][20] = {};
                n = read(variables.intr[j].fd, buffer, 128);
                if (n == 0)
                {
                    printf("Perdeu conexao com o interno\n");
                    close(variables.intr[j].fd);
                    FD_CLR(variables.intr[j].fd, &rfds);
                    memset(&variables.intr[j], 0, sizeof(variables.intr[j]));
                    variables.intr[j].fd = -1;
                    break;
                }
                else
                {
                    sscanf(buffer, "%s %s %s %s", message[0], message[1], message[2], message[3]);
                    write(1, "received: ", 11);
                    write(1, buffer, strlen(buffer) + 1);
                    if (strcmp(message[0], "NEW") == 0)
                    {
                        strcpy(variables.intr[j].id, message[1]);
                        strcpy(variables.intr[j].ip, message[2]);
                        strcpy(variables.intr[j].tcp, message[3]);
                        variables.head = insertAtEnd(variables.head,variables.intr[j].id,variables.intr[j].id);//Adicionar novo nó na tabela de expediçao
                        SendExtern(variables.intr[j].fd, variables);
                    }
                }
            }
        }

        if (FD_ISSET(variables.ext.fd, &rfds))
        {
            int n;
            char buffer[128] = {};
            char message[4][20] = {};
            n = read(variables.ext.fd, buffer, 128);

            if (n == 0)
            {
                printf("Perdeu conexao com o externo\n");
                close(variables.ext.fd);
                FD_CLR(variables.ext.fd, &rfds);

                if (strcmp(variables.id, variables.bck.id) == 0)
                {
                    int viz;
                    viz = 0;
                    // Para ir buscar o primeiro vizinho interno
                    for (viz; viz != 99; viz++)
                        if (variables.intr[viz].fd != -1)
                            break;

                    if (variables.intr[viz].fd != -1 && viz != 99)
                    {
                        strcpy(variables.ext.id, variables.intr[viz].id);
                        strcpy(variables.ext.ip, variables.intr[viz].ip);
                        strcpy(variables.ext.tcp, variables.intr[viz].tcp);
                        variables.ext.fd = variables.intr[viz].fd;
                        for (int j = 0; j != 99; j++)
                        {
                            if (variables.intr[j].fd != -1)
                            {
                                SendExtern(variables.intr[j].fd, variables);
                            }
                        }
                        memset(&variables.intr[viz], 0, sizeof(variables.intr[viz]));
                        variables.intr[viz].fd = -1;
                    }
                    else
                    {
                        first_node = 1;
                        strcpy(variables.ext.id, variables.id);
                        strcpy(variables.ext.ip, info.ip);
                        strcpy(variables.ext.tcp, info.tcp);
                        variables.ext.fd = -1;
                    }
                }
                else
                {
                    variables.ext.fd = EstablishConnection(variables.bck.ip, variables.bck.tcp, info);
                    if (variables.ext.fd > maxfd)
                        maxfd = variables.ext.fd;
                    FD_SET(variables.ext.fd, &rfds);
                    strcpy(variables.ext.id, variables.bck.id);
                    strcpy(variables.ext.ip, variables.bck.ip);
                    strcpy(variables.ext.tcp, variables.bck.tcp);
                    SendNew(variables.ext.fd, info);
                }
                for (int k = 0; k != 99; k++)
                {
                    if (variables.intr[k].fd != -1)
                        SendExtern(variables.intr[k].fd, variables);
                }
            }
            else
            {
                sscanf(buffer, "%s %s %s %s", message[0], message[1], message[2], message[3]);
                write(1, "received: ", 11);
                write(1, buffer, strlen(buffer) + 1);
                if (strcmp(message[0], "NEW") == 0)
                {
                    strcpy(variables.ext.id, message[1]);
                    strcpy(variables.ext.ip, message[2]);
                    strcpy(variables.ext.tcp, message[3]);
                    variables.head = insertAtEnd(variables.head,variables.ext.id,variables.ext.id);//Adicionar o novo nó na tabela de expediçao
                    SendExtern(variables.ext.fd, variables);
                    FD_SET(variables.ext.fd, &rfds); // acho q nao e preciso
                }
                else if (strcmp(message[0], "EXTERN") == 0)
                {
                    strcpy(variables.bck.id, message[1]);
                    strcpy(variables.bck.ip, message[2]);
                    strcpy(variables.bck.tcp, message[3]);
                    if(strcmp(message[1],variables.id) != 0)
                        variables.head = insertAtEnd(variables.head,variables.bck.id,variables.ext.id);
                }
            }
        }
    }
    return 0;
}