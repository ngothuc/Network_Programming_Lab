#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account.h"

#define ACTIVE_STATUS 1
#define BLOCK_STATUS 0

Account *accountListHead = NULL;
Account *loginAccount = NULL;

void readAccountFromFile()
{
    FILE *file = fopen("account.txt", "r");
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
        // newAccount->loginError = 0;
        newAccount->next = accountListHead;
        accountListHead = newAccount;
    }
    fclose(file);
}

int checkAccount(char *username, char *password)
{
    for (Account *tmp = accountListHead; tmp != NULL; tmp = tmp->next)
    {
        if (strcmp(tmp->username, username) == 0 && strcmp(tmp->password, password) == 0)
        // Nhập đúng cả tài khoản và mật khẩu
        {
            if (tmp->status == ACTIVE_STATUS)
            {
                // chưa bị khóa thì trả về 1
                login(tmp);
                tmp->loginError = 0;
                updateAccountFile(tmp);
                return 1;
            }
            else
            {
                // bị khóa thì trả về -1
                return -1;
            }
        }
        if (strcmp(tmp->username, username) == 0)
        {
            // đúng tài khoản, sai mật khẩu
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
    // mặc định trả về 0
    return 0;
}

void updateAccountFile(Account *account)
{

    char username[50], password[50];
    int loginError, status;

    FILE *file = fopen("account.txt", "r");
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

    remove("account.txt");
    rename("temp.txt", "account.txt");
    return;
}

void login(Account *account)
{
    loginAccount = account;
}

void logout()
{
    loginAccount = NULL;
}
