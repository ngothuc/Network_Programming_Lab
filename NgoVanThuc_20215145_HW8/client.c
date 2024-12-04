#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "account.h"

#define BUFF_SIZE 1024

char *receiveMessage(int sockfd)
{
    char buffer[BUFF_SIZE];
    int bytes_received = recv(sockfd, buffer, BUFF_SIZE, 0);
    if (bytes_received <= 0)
    {
        return NULL;
    }
    buffer[bytes_received] = '\0';
    return strdup(buffer);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <IPAddress> <PortNumber>\n", argv[0]);
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    char buffer[BUFF_SIZE];
    char username[50], password[50];

    while (1)
    {
        printf("Enter username: ");
        scanf("%s", username);
        send(sockfd, username, strlen(username), 0);

        char *response = receiveMessage(sockfd);
        printf("%s\n", response);

        if (strcmp(response, "Enter password: ") == 0)
        {
            while (1)
            {
                printf("Enter password: ");
                scanf("%s", password);
                send(sockfd, password, strlen(password), 0);

                char *tmp = receiveMessage(sockfd);
                printf("%s\n", tmp);

                if (strcmp(tmp, "OK") == 0)
                {
                    printf("Login successful. Enter 'logout' to logout or newpassword to change password: ");
                    while (1)
                    {
                        scanf("%s", buffer);
                        send(sockfd, buffer, strlen(buffer), 0);

                        char *command_response = receiveMessage(sockfd);
                        printf("Response from server: %s\n", command_response);

                        if (strcmp(command_response, "Logout successful\n") == 0)
                        {
                            printf("Logout successfully\n");
                            close(sockfd);
                            return 0;
                        }
                        else if (strcmp(command_response, "Invalid password format\n") == 0)
                        {
                            printf("Invalid password format. Password must contain only letters and digits.\n");
                        }
                        else if (strcmp(command_response, "Changed password\n") == 0)
                        {
                            char *letters = receiveMessage(sockfd);
                            char *digits = receiveMessage(sockfd);
                            printf("Password updated successfully.\nLetters: %s\nDigits: %s\n", letters, digits);
                            free(letters);
                            free(digits);
                        }
                    }
                }
                else if (strcmp(tmp, "Invalid password") == 0)
                {
                    printf("Invalid password. Try again.\n");
                }
                else if (strcmp(tmp, "Account blocked") == 0)
                {
                    printf("Account is blocked. Exiting...\n");
                    close(sockfd);
                    return 0;
                }
                else if (strcmp(tmp, "Login failed 3 times, account blocked") == 0)
                {
                    printf("Login failed 3 times, account blocked. Exiting...\n");
                    close(sockfd);
                    return 0;
                }
            }
        }
        else if (strcmp(response, "Account not found") == 0)
        {
            printf("Account not found. Try again.\n");
        }
    }

    close(sockfd);
    return 0;
}