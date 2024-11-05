#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "account.h"

#define BUFF_SIZE 1024

void handle_client(int client_sock) {
    char buffer[BUFF_SIZE];
    char username[50], password[50];

    while (1) {
        int bytes_received = recv(client_sock, buffer, BUFF_SIZE, 0);
        if (bytes_received <= 0) break;

        buffer[bytes_received] = '\0';

        if (strcmp(buffer, "logout") == 0) {
            logout();
            strcpy(buffer, "Logout successful");
            send(client_sock, buffer, strlen(buffer), 0);
            
        }

        sscanf(buffer, "%s %s", username, password);

        int status = checkAccount(username, password);
        if (status == 1) {
            strcpy(buffer, "OK");
        } else if (status == 0) {
            strcpy(buffer, "Account not found");
        } else if (status == 2) {
            strcpy(buffer, "Not OK");
        } else if (status == 3) {
            strcpy(buffer, "Account blocked");
        } else {
            strcpy(buffer, "Account not ready");
        }

        send(client_sock, buffer, strlen(buffer), 0);

        if (status == 1 || status == 3) break;
    }
    close(client_sock);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <PortNumber>\n", argv[0]);
        return 1;
    }

    readAccountFromFile();

    int port = atoi(argv[1]);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen = sizeof(cliaddr);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    listen(sockfd, 5);

    while (1) {
        int client_sock = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen);
        if (fork() == 0) {
            close(sockfd);
            handle_client(client_sock);
            exit(0);
        } else {
            close(client_sock);
        }
    }

    close(sockfd);
    return 0;
}
