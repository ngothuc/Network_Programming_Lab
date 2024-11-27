#ifndef ACCOUNT_H
#define ACCOUNT_H

typedef struct Account {
    char username[50];
    char password[50];
    int status;
    int loginError;
    struct Account* next;
} Account;

void readAccountFromFile();
int checkAccount(char* username, char* password);
void updateAccountFile(Account* account);
void login(Account* account);
void logout();

extern Account* accountListHead;
extern Account* loginAccount;

#endif
