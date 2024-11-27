#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

int isValidIP(char *ip)
{
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0;
}

void lookupByIP(char *ip)
{
    struct hostent *host;
    struct in_addr addr;

    if (!isValidIP(ip))
    {
        printf("No information found\n");
        return;
    }

    inet_pton(AF_INET, ip, &addr);
    host = gethostbyaddr(&addr, sizeof(addr), AF_INET);

    if (host == NULL)
    {
        printf("No information found\n");
    }
    else
    {
        printf("Main name: %s\n", host->h_name);
        printf("Alternate names: ");
        if (host->h_aliases[0] != NULL)
        {            
            for (char **alias = host->h_aliases; *alias != NULL; alias++)
            {
                printf("%s", *alias);
            }
        }
        printf("\n");
    }
}

void lookupByName(char *domain)
{
    struct hostent *host = gethostbyname(domain);

    if (host == NULL)
    {
        printf("No information found\n");
    }
    else
    {
        printf("Main IP: %s\n", inet_ntoa(*(struct in_addr *)host->h_addr));
        printf("Alternate IP:");
        if (host->h_aliases[0] != NULL)
        {
            for (char **alias = host->h_aliases; *alias != NULL; alias++)
            {
                printf("%s\n", inet_ntoa(*(struct in_addr *)*alias));
            }
        }
        printf("\n");
    }
}
