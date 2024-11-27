#ifndef ACCOUNT_H
#define ACCOUNT_H

typedef struct Account {
    char username[50];
    char password[50];
    int status;
    int loginError;
    int client_sock;
    struct Account* next;
} Account;

void readAccountFromFile();
int checkAccount(char* username, char* password, int client_sock);
void updateAccountFile(Account* account);
void login(Account* account);
void logout(int client_sock);

#endif