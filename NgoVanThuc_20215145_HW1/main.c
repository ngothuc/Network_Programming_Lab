#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account.h"

void printMenu()
{
    printf("USER MANAGEMENT PROGRAM\n");
    printf("-----------------------------------\n");
    printf("1. Register\n");
    printf("2. Sign in\n");
    printf("3. Change password\n");
    printf("4. Update account info\n");
    printf("5. Reset password\n");
    printf("6. View login history\n");
    printf("7. Sign out\n");
    printf("Your choice (1-7, other to quit): ");
}

int main()
{
    readUser();

    int action = 0;

    while (1)
    {
        action = 0;
        printMenu();
        scanf("%d", &action);

        switch (action)
        {
        case 1:
            registerAccount();
            break;
        case 2:
            login();
            break;
        case 3:
            changePassword();
            break;
        case 4:
            changeAccountInfo();
            break;
        case 5:
            resetPassword();
            break;
        case 6:
            viewLoginHistory();
            break;
        case 7:
            logout();
            break;

        default:
            freeUserList();
            return 0;
        }
    }

    return 0;
}