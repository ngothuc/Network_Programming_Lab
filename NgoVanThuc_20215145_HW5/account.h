#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_LENGTH 255
#define IDLE_STATUS 2
#define ACTIVE_STATUS 1
#define BLOCK_STATUS 0

char username[STR_LENGTH];
char password[STR_LENGTH];
char email[STR_LENGTH];
char phone[STR_LENGTH];
int status;
char homepage[STR_LENGTH];
int loginError;

typedef struct user
{
    char username[STR_LENGTH];
    char password[STR_LENGTH];
    char email[STR_LENGTH];
    char phone[STR_LENGTH];
    int status;
    char homepage[STR_LENGTH];
    int loginError;
    struct user *next;
} Account;

Account *loginAccount = NULL;
Account *accountListHead = NULL;
Account *accountListTail = NULL;

Account *newAccount(char *username, char *password, char *email, char *phone, int status, char *homepage, int loginError)
{
    Account *temp = (Account *)malloc(sizeof(Account));
    strcpy(temp->username, username);
    strcpy(temp->password, password);
    strcpy(temp->email, email);
    strcpy(temp->phone, phone);
    temp->status = status;
    strcpy(temp->homepage, homepage);
    temp->loginError = loginError;
    temp->next = NULL;
    return temp;
}

void addNewAccountToFile(Account *account)
{
    FILE *fp = fopen("nguoidung.txt", "a+");
    if (fp == NULL)
    {
        printf("Error opening file.\n");
        return;
    }
    fprintf(fp, "%s %s %s %s %d %s %d\n",
            account->username, account->password, account->email, account->phone,
            account->status, account->homepage, account->loginError);
    fclose(fp);
}

void updateAccountFile(Account *user)
{
    FILE *file = fopen("nguoidung.txt", "r");
    FILE *tmpFile = fopen("temp.txt", "w");
    if (file == NULL || tmpFile == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    while (fscanf(file, "%s %s %s %s %d %s %d", username, password, email, phone, &status, homepage, &loginError) != EOF)
    {
        if (strcmp(username, user->username) != 0)
        {
            fprintf(tmpFile, "%s %s %s %s %d %s %d\n", username, password, email, phone, status, homepage, loginError);
        }
        else
        {
            fprintf(tmpFile, "%s %s %s %s %d %s %d\n", user->username, user->password, user->email, user->phone, user->status, user->homepage, user->loginError);
        }
    }

    fclose(file);
    fclose(tmpFile);

    remove("nguoidung.txt");
    rename("temp.txt", "nguoidung.txt");
    return;
}

void readAccountFromFile()
{
    FILE *fp = fopen("nguoidung.txt", "r");
    if (fp == NULL)
    {
        printf("File not found.\n");
        return;
    }
    while (fscanf(fp, "%s %s %s %s %d %s %d", username, password, email, phone, &status, homepage, &loginError) != EOF)
    {
        if (accountListHead != NULL)
        {
            accountListTail->next = newAccount(username, password, email, phone, status, homepage, loginError);
            accountListTail = accountListTail->next;
        }
        else
        {
            accountListHead = newAccount(username, password, email, phone, status, homepage, loginError);
            accountListTail = accountListHead;
        }
    }
    fclose(fp);
}

void registerNewAccount()
{
    printf("Register Form \n");
    printf("Enter your username: ");
    scanf("%s", username);
    printf("Enter your password: ");
    scanf("%s", password);
    printf("Enter your email: ");
    scanf("%s", email);
    printf("Enter your phone number: ");
    scanf("%s", phone);
    printf("Enter your homepage: ");
    scanf("%s", homepage);
    loginError = 0;

    Account *tmp = newAccount(username, password, email, phone, IDLE_STATUS, homepage, loginError);

    addNewAccountToFile(tmp);

    if (accountListHead == NULL)
    {
        accountListHead = tmp;
        accountListTail = tmp;
    }
    else
    {
        accountListTail->next = tmp;
        accountListTail = tmp;
    }

    printf("Account is registered\n");
}

void login(Account *account)
{
    loginAccount = account;
    if (loginAccount->status == IDLE_STATUS)
    {
        loginAccount->status = ACTIVE_STATUS;
    }
}

void logout()
{
    loginAccount = NULL;
}

void updatePassword(char *newPassword)
{
    if (loginAccount == NULL)
        return;
    strcpy(loginAccount->password, newPassword);
    updateAccountFile(loginAccount);
}
