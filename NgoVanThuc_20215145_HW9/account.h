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

Account* readAccountFromFile(char* filename);
Account* findAccount(char* username);
int checkAccount(char* username, char* password, int client_sock);
void updateAccountFile(Account* account);
void updateAccountPassword(char* username, char* newPassword);


#endif