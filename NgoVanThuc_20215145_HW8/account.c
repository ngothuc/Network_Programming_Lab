#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account.h"

#define ACTIVE_STATUS 1
#define BLOCK_STATUS 0

Account *accountListHead = NULL;

void readAccountFromFile()
{
    FILE *file = fopen("user.txt", "r");
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
}

int findAccount(char *username) {
    for (Account *tmp = accountListHead; tmp != NULL; tmp = tmp->next) {
        if (strcmp(tmp->username, username) == 0) {
            return 1;
        }
    }
    return 0;
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

    FILE *file = fopen("user.txt", "r");
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

    remove("user.txt");
    rename("temp.txt", "user.txt");
    return;
}

void login(Account *account)
{
    // TO DO
}

void logout(int client_sock)
{
    for (Account *tmp = accountListHead; tmp != NULL; tmp = tmp->next)
    {
        if (tmp->client_sock == client_sock)
        {
            tmp->client_sock = -1; // Đặt lại client_sock khi logout
            break;
        }
    }
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