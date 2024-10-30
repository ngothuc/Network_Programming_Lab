#include <stdio.h>
#include "account.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#define BUFF_SIZE 1024

void encryptPassword(char* password, char* lettersOnly, char* digitsOnly) {
    int l_idx = 0, d_idx = 0;
    for (int i = 0; i < strlen(password); i++) {
        if (isalpha(password[i])) {
            lettersOnly[l_idx++] = password[i];
        } else if (isdigit(password[i])) {
            digitsOnly[d_idx++] = password[i];
        }
    }
    lettersOnly[l_idx] = '\0';
    digitsOnly[d_idx] = '\0';
}

int isPasswordValid(char* password) {
    for (int i = 0; i < strlen(password); i++) {
        if (!isalnum(password[i])) {
            return 0;
        }
    }
    return 1;
}

int checkAccount(char *username, char *password) {
    for (Account *tmp = accountListHead; tmp != NULL; tmp = tmp->next) {
        if (strcmp(tmp->username, username) == 0 && strcmp(tmp->password, password) == 0) {
            if (tmp->status == ACTIVE_STATUS || tmp->status == IDLE_STATUS) {
                login(tmp);
                tmp->loginError = 0;
                updateAccountFile(tmp);
                return 1;
            } else {
                return -1;
            }
        } else {
            if (strcmp(tmp->username, username) == 0) {
                tmp->loginError++;
                if (tmp->loginError == 3) {
                    tmp->status = BLOCK_STATUS;
                    updateAccountFile(tmp);
                    return 3;
                }
                updateAccountFile(tmp);
                return 2; 
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <PortNumber>\n", argv[0]);
        return 1;
    }

    readAccountFromFile();

    int port = atoi(argv[1]);
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[BUFF_SIZE];
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    socklen_t len = sizeof(cliaddr);
    while (1) {
        int n = recvfrom(sockfd, buffer, BUFF_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';

        printf("Received from client: %s\n", buffer);

        if (loginAccount == NULL) {
            char username[255], password[255];
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

            sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&cliaddr, len);
        } else {
            if (strcmp(buffer, "bye") == 0) {
                logout();
                strcpy(buffer, "Goodbye");
                sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&cliaddr, len);
            } else if (strcmp(buffer, "homepage") == 0) {
                strcpy(buffer, loginAccount->homepage);
                sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&cliaddr, len);
            } else {
                if (isPasswordValid(buffer)) {
                    char lettersOnly[BUFF_SIZE] = "";
                    char digitsOnly[BUFF_SIZE] = "";
                    encryptPassword(buffer, lettersOnly, digitsOnly);

                    updatePassword(buffer); 

                    char message[BUFF_SIZE];
                    int length = snprintf(message, BUFF_SIZE, "Password updated: Letters [%s], Digits [%s]", lettersOnly, digitsOnly);

                    if (length >= BUFF_SIZE) {
                        snprintf(buffer, BUFF_SIZE, "Password updated: Letters [%.*s], Digits [%.*s]", 
                                 BUFF_SIZE - 40, lettersOnly, BUFF_SIZE - 40, digitsOnly);
                    } else {
                        strcpy(buffer, message);
                    }
                } else {
                    strcpy(buffer, "Error: Password must contain only letters and digits.");
                }
                sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&cliaddr, len);
            }
        }
    }

    close(sockfd);
    return 0;
}
