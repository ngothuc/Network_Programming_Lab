#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <ctype.h>
#include "account.h"

#define BUFF_SIZE 1024

char *receiveMessage(int client_sock)
{
    char buffer[BUFF_SIZE];
    int bytes_received = recv(client_sock, buffer, BUFF_SIZE, 0);
    if (bytes_received <= 0)
        return NULL;
    buffer[bytes_received] = '\0';
    return strdup(buffer);
}

int isValidPassword(char *password)
{
    for (int i = 0; i < strlen(password); i++)
    {
        char c = password[i];
        if (!isalnum(c))
        {
            return 0;
        }
    }
    return 1;
}

void splitPassword(char *password, char *letters, char *digits)
{
    int l = 0, d = 0;
    for (int i = 0; i < strlen(password); i++)
    {
        char c = password[i];
        if (isalpha(c))
        {
            letters[l++] = c;
        }
        else if (isdigit(c))
        {
            digits[d++] = c;
        }
    }
    letters[l] = '\0';
    digits[d] = '\0';
}

void *handle_client(void *arg)
{
    int client_sock = *(int *)arg;
    free(arg);
    char buffer[BUFF_SIZE];
    char username[50], password[50];

    while (1)
    {
        int bytes_received = recv(client_sock, buffer, BUFF_SIZE, 0);
        if (bytes_received <= 0)
            break;
        buffer[bytes_received] = '\0';

        int target = findAccount(buffer);
        if (target == 1)
        {
            strcpy(username, buffer);
            printf("Account %s wants to login from port %d\n", buffer, client_sock);

            int status;
            while (1)
            {
                send(client_sock, "Enter password: ", 16, 0);
                char *password = receiveMessage(client_sock);
                status = checkAccount(username, password, client_sock);

                if (status == 2)
                {
                    send(client_sock, "Invalid password", 16, 0);
                }
                else
                {
                    break;
                }
            }

            if (status == 1)
            {
                send(client_sock, "OK", 2, 0);
                printf("Client %d login successfully\n", client_sock);

                while (1)
                {
                    char *command = receiveMessage(client_sock);
                    printf("Client %d send command: %s\n", client_sock, command);
                    if (strcmp(command, "logout") == 0)
                    {
                        printf("Client %d logout\n", client_sock);
                        logout(client_sock);
                        send(client_sock, "Logout successful\n", 18, 0);
                        break;
                    }
                    else
                    {
                        if (isValidPassword(command))
                        {
                            printf("Account %s send update password to %s\n", username, command);
                            updateAccountPassword(username, command);
                            char letters[BUFF_SIZE], digits[BUFF_SIZE];
                            splitPassword(command, letters, digits);
                            char response[BUFF_SIZE];
                            strcat(response, "Changed password\n");
                            // send(client_sock, letters, strlen(letters), 0);
                            // send(client_sock, digits, strlen(digits), 0);
                            strcat(response, "Letters: "); strcat(response, letters);
                            strcat(response, "\nDigits: "); strcat(response, digits);
                            send(client_sock, response, strlen(response), 0);
                        }
                        else
                        {
                            send(client_sock, "Invalid password format\n", 24, 0);
                        }
                    }
                }
            }
            else if (status == -1)
            {
                send(client_sock, "Account blocked", 15, 0);
                printf("Client %d login failed: Account blocked\n", client_sock);
            }
            else if (status == 3)
            {
                send(client_sock, "Login failed 3 times, account blocked", 35, 0);
                printf("Client %d login failed 3 times, account blocked\n", client_sock);
            }
        }
        else
        {
            send(client_sock, "Account not found", 17, 0);
        }
    }
    close(client_sock);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <PortNumber>\n", argv[0]);
        return 1;
    }

    readAccountFromFile();

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);

    printf("Server is listening on port %d\n", atoi(argv[1]));

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);

        int *new_sock = (int *)malloc(sizeof(int));
        *new_sock = client_sock;
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, (void *)new_sock);
        pthread_detach(thread);
    }

    close(server_sock);
    return 0;
}