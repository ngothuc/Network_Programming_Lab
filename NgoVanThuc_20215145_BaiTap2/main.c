#include <stdio.h>
#include <stdlib.h>
#include "domain.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./lookup <option> <parameter>\n");
        return 1;
    }

    int option = atoi(argv[1]);
    char *parameter = argv[2];

    if (option == 1) {
        if (isValidIP(parameter)) {
            lookupByIP(parameter);
        } else {
            printf("No information found \n");
        }
    } else if (option == 2) {
        if (isValidIP(parameter)) {
            printf("Invalid option\n");
        } else {
            lookupByName(parameter);
        }
    } else {
        printf("Invalid option\n");
    }

    return 0;
}
