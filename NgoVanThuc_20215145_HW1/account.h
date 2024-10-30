#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ACTIVE 1
#define BLOCK 0
#define DEFAULT_OTP "OTP"

typedef struct user
{
    char username[256];
    char password[256];
    char email[256];
    char phone[256];
    int status;
    struct user *next;
} User;

User *head = NULL;
User *loginAccount = NULL;

User *createUser(char *username, char *password, char *email, char *phone)
{
    User *newUser = (User *)malloc(sizeof(User));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    strcpy(newUser->email, email);
    strcpy(newUser->phone, phone);
    newUser->status = ACTIVE;
    newUser->next = NULL;
    return newUser;
}

void readUser()
{
    char username[256], password[256], email[256], phone[256];
    int status;
    FILE *fp = fopen("account.txt", "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        return;
    }
    User *tmp = NULL;

    while (fscanf(fp, "%s %s %s %s %d", username, password, email, phone, &status) != EOF)
    {
        if (head == NULL)
        {
            head = createUser(username, password, email, phone);
            head->status = status;
            tmp = head;
        }
        else
        {
            tmp->next = createUser(username, password, email, phone);
            tmp->next->status = status;
            tmp = tmp->next;
        }
    }

    fclose(fp);
}

void registerAccount()
{

    char username[256];
    char password[256];
    char email[256];
    char phone[256];

    printf("Register\n");
    printf("-----------------------------------\n");
    printf("Enter your username: ");
    scanf("%s", username);
    fflush(stdin);
    for (User *tmp = head; tmp != NULL; tmp = tmp->next)
    {
        if (strcmp(tmp->username, username) == 0)
        {
            printf("Username already exists\n");
            return;
        }
    }
    printf("Enter your password: ");
    scanf("%s", password);
    fflush(stdin);
    printf("Enter your email: ");
    scanf("%s", email);
    fflush(stdin);
    printf("Enter your phone: ");
    scanf("%s", phone);
    fflush(stdin);
    printf("-----------------------------------\n");

    FILE *fp = NULL;
    fp = fopen("account.txt", "a");
    if (fp == NULL) {
        printf("Error opening file\n");
        return;
    }
    fprintf(fp, "%s %s %s %s %d\n", username, password, email, phone, ACTIVE);
    fclose(fp);

    User *tmp = head;
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = createUser(username, password, email, phone);

    printf("Registration successful!\n");
}

void updateAccountFile(User* user) {
    FILE *file = fopen("account.txt", "r");
    FILE *tmpFile = fopen("temp.txt", "w");
    if (file == NULL || tmpFile == NULL) {
        printf("Error opening file.\n");
        return;
    }

    char username[256];
    char password[256];
    char email[256];
    char phone[256];
    int status;

    while (fscanf(file, "%s %s %s %s %d", username, password, email, phone, &status) != EOF)
    {
        if (strcmp(username, user->username) != 0)
        {
            fprintf(tmpFile, "%s %s %s %s %d\n", username, password, email, phone, status);
        } else {
            fprintf(tmpFile, "%s %s %s %s %d\n", user->username, user->password, user->email, user->phone, user->status);
        }
    }


    fclose(file);
    fclose(tmpFile);

    remove("account.txt");
    rename("temp.txt", "account.txt");
    return;
}

void login()
{

    char username[256];
    char password[256];
    int count = 0;
    User *account = NULL;

    printf("Login\n");
    printf("-----------------------------------\n");
    printf("Enter your username: ");
    scanf("%s", username);
    fflush(stdin);
    for (User *tmp = head; tmp != NULL; tmp = tmp->next)
    {
        if (strcmp(tmp->username, username) == 0)
        {
            if(tmp->status == BLOCK) {
                printf("Your account is blocked.\n");
                return;
            }
            account = tmp;
            break;
        }
    }
    if (account == NULL)
    {
        printf("Account does not exist\n");
        return;
    }
    while (1)
    {
        printf("Enter your password: ");
        scanf("%s", password);
        fflush(stdin);
        if (strcmp(account->password, password) == 0)
        {
            loginAccount = account;

            FILE *fp = fopen("history.txt", "a");
            if(fp == NULL) {
                printf("Error opening file.\n");
                return;
            }



            time_t currentTime;
            time(&currentTime);
            struct tm *localTime = localtime(&currentTime);
            char timeString[50];
            strftime(timeString, sizeof(timeString), "%Y-%m-%d | %H:%M:%S", localTime);
            fprintf(fp, "%s | %s\n", loginAccount->username, timeString);



            fclose(fp);

            printf("Welcome\n");
            break;
        }
        else
        {
            printf("Incorrect password. Please try again.\n");
            count++;
            if (count > 3)
            {
                printf("Your account is blocked.\n");
                account->status = BLOCK;
                updateAccountFile(account);
                break;
            }
        }
    }
}

void changePassword() {
    if (loginAccount == NULL)
    {
        printf("You are not logged in.\n");
        return;
    }
    char oldPassword[256];
    char newPassword[256];

    printf("Change password\n");
    printf("-----------------------------------\n");
    printf("Enter your old password: ");
    scanf("%s", oldPassword);
    fflush(stdin);
    if (strcmp(loginAccount->password, oldPassword)!= 0)
    {
        printf("Incorrect old password.\n");
        return;
    }
    printf("Enter your new password: ");
    scanf("%s", newPassword);
    fflush(stdin);
    strcpy(loginAccount->password, newPassword);
    updateAccountFile(loginAccount);
    printf("-----------------------------------\n");
}

void changeAccountInfo() {
    if (loginAccount == NULL)
    {
        printf("You are not logged in.\n");
        return;
    }

    printf("Change info\n");
    printf("-----------------------------------\n");
    printf("1. Change email\n");
    printf("2. Change phone\n");
    int choice = 0;
    printf("Your choice (1-2, other to quit): ");
    scanf("%d", &choice);

    if(choice != 1 && choice != 2) return;
    if(choice == 1) {
        char newEmail[256];
        printf("Enter your new email: ");
        scanf("%s", newEmail);
        fflush(stdin);
        strcpy(loginAccount->email, newEmail);
        updateAccountFile(loginAccount);
        return;
    }
    if (choice == 2)
    {
        char newPhone[256];
        printf("Enter your new phone: ");
        scanf("%s", newPhone);
        fflush(stdin);
        strcpy(loginAccount->phone, newPhone);
        updateAccountFile(loginAccount);
        return;
    }
    
}

void resetPassword() {
    printf("Reset Password\n");
    printf("-----------------------------------\n");
    char username[256];
    printf("Enter your username: ");
    scanf("%s", username);
    fflush(stdin);
    User *target = NULL;
    for(User* tmp = head; tmp != NULL; tmp = tmp->next) {
        if(strcmp(tmp->username, username) == 0) {
            target = tmp;
            break;
        }
    }
    if (target == NULL)
    {
        printf("Account does not exist");
        return;
    }
    char otp[256];
    printf("Enter OTP sent to your email (Default is %s): ", DEFAULT_OTP);
    scanf("%s", otp);
    fflush(stdin);

    if(strcmp(otp, DEFAULT_OTP) == 0) {
        printf("Enter your new password: ");
        char newPassword[256];
        scanf("%s", newPassword);
        fflush(stdin);
        strcpy(target->password, newPassword);
        updateAccountFile(target);
        printf("Password reset successful.\n");
        return;
    } else {
        printf("OTP is incorrect\n");
        return;
    }
    
}

void logout() {
    if(loginAccount == NULL) {
        printf("You are not logged in.\n");
        return;
    }
    loginAccount = NULL;
    printf("Logout successful.\n");
}

void viewLoginHistory() {
    if(loginAccount == NULL) {
        printf("You are not logged in.\n");
        return;
    }
    printf("Login History\n");
    printf("-----------------------------------\n");
    printf("|   %-15s |   %-10s |   %-10s|\n", "User Name", "Login Day", "Login Time");
    printf("-----------------------------------\n");
    
    FILE *fp = fopen("history.txt", "r");
    if(fp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    char username[256];
    char loginDay[256];
    char loginTime[256];

    while (fscanf(fp, "%s | %s | %s", username, loginDay, loginTime) != EOF)
    {
        if(strcmp(username, loginAccount->username) == 0) {
            printf("|   %-15s |   %-10s |   %-10s|\n", username, loginDay, loginTime);
        }
    }
    printf("-----------------------------------\n");

    fclose(fp);
}

void freeUserList() {
    User *tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
    return;
}