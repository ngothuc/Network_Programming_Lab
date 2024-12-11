#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account.h"

#define ACTIVE_STATUS 1
#define BLOCK_STATUS 0
#define MAX_CLIENTS 1024


Account *accountListHead = NULL;

Account* readAccountFromFile(char* filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open account file");
        exit(1);
    }

    Account *newAccount;
    while (!feof(file))
    {
        newAccount = (Account *)malloc(sizeof(Account));
        fscanf(file, "%s %s %d %d", newAccount->username, newAccount->password, &newAccount->status, &newAccount->loginError);
        newAccount->client_sock = -1; // Khởi tạo client_sock là -1
        newAccount->next = accountListHead;
        accountListHead = newAccount;
    }
    fclose(file);
    return accountListHead;
}

Account* findAccount(char *username) {
    for (Account *tmp = accountListHead; tmp != NULL; tmp = tmp->next) {
        if (strcmp(tmp->username, username) == 0) {
            return tmp;
        }
    }
    return NULL;
}

int checkAccount(char *username, char *password, int client_sock)
{
    for (Account *tmp = accountListHead; tmp != NULL; tmp = tmp->next)
    {
        if (strcmp(tmp->username, username) == 0 && strcmp(tmp->password, password) == 0)
        {
            if (tmp->status == ACTIVE_STATUS)
            {
                tmp->client_sock = client_sock; // Gán client_sock cho tài khoản
                tmp->loginError = 0;
                updateAccountFile(tmp);
                return 1;
            }
            else
            {
                return -1;
                
            }
        }
        if (strcmp(tmp->username, username) == 0)
        {
            printf("True password: %s\n", tmp->password);
            tmp->loginError++;
            if (tmp->loginError >= 3)
            {
                tmp->status = BLOCK_STATUS;
                updateAccountFile(tmp);
                return 3;
            }
            updateAccountFile(tmp);
            return 2;
        }
    }
    return 0;
}

void updateAccountFile(Account *account)
{
    char username[50], password[50];
    int loginError, status;

    FILE *file = fopen("nguoidung.txt", "r");
    FILE *tmpFile = fopen("temp.txt", "w");
    if (file == NULL || tmpFile == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    while (fscanf(file, "%s %s %d %d", username, password, &status, &loginError) != EOF)
    {
        if (strcmp(username, account->username) != 0)
        {
            fprintf(tmpFile, "%s %s %d %d\n", username, password, status, loginError);
        }
        else
        {
            fprintf(tmpFile, "%s %s %d %d\n", account->username, account->password, account->status, account->loginError);
        }
    }

    fclose(file);
    fclose(tmpFile);

    remove("nguoidung.txt");
    rename("temp.txt", "nguoidung.txt");
    return;
}



void updateAccountPassword(char *username, char *newPassword)
{
    for (Account *tmp = accountListHead; tmp != NULL; tmp = tmp->next)
    {
        if (strcmp(tmp->username, username) == 0)
        {
            strcpy(tmp->password, newPassword);
            updateAccountFile(tmp);
            break;
        }
    }
}