#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "account.h"

#define BUFF_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IPAddress> <PortNumber>\n", argv[0]);
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);

    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    char buffer[BUFF_SIZE];
    char username[50], password[50];

    while (1) {
        printf("Enter username: ");
        scanf("%s", username);
        printf("Enter password: ");
        scanf("%s", password);

        sprintf(buffer, "%s %s", username, password);
        send(sockfd, buffer, strlen(buffer), 0);

        int bytes_received = recv(sockfd, buffer, BUFF_SIZE, 0);
        buffer[bytes_received] = '\0';
        printf("Response from server: %s\n", buffer);

        if (strcmp(buffer, "OK") == 0) {
            printf("Enter 'logout' to logout: ");
            scanf("%s", buffer);
            send(sockfd, buffer, strlen(buffer), 0);

            bytes_received = recv(sockfd, buffer, BUFF_SIZE, 0);
            buffer[bytes_received] = '\0';
            printf("Response from server: %s\n", buffer);

            if (strcmp(buffer, "Logout successful") == 0) {
                printf("Logout successfully\n");
                close(sockfd);
                return 0;
            }
        } else if (strcmp(buffer, "Account blocked") == 0) {
            break;
        }
    }

    close(sockfd);
    return 0;
}