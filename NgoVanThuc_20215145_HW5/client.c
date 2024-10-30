#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IPAddress> <PortNumber>\n", argv[0]);
        return 1;
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUFF_SIZE];
    char username[50], password[50];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Enter username: ");
        scanf("%s", username); 
        fflush(stdin);
        printf("Enter password: ");
        scanf("%s", password); 
        fflush(stdin);

        sprintf(buffer, "%s %s", username, password);
        send(sockfd, buffer, strlen(buffer), 0);

        int n = recv(sockfd, buffer, BUFF_SIZE, 0);
        buffer[n] = '\0';
        printf("Response from server: %s\n", buffer);

        if (strcmp(buffer, "OK") == 0) {
            while (1) {
                printf("Enter new password, 'homepage' to view homepage, 'bye' to logout: ");
                scanf("%s", buffer);
                fflush(stdin);
                send(sockfd, buffer, strlen(buffer), 0);

                n = recv(sockfd, buffer, BUFF_SIZE, 0);
                buffer[n] = '\0';
                printf("Response from server: %s\n", buffer);

                if (strcmp(buffer, "Goodbye") == 0) {
                    break;
                }
            }
        }
    }

    close(sockfd);
    return 0;
}
