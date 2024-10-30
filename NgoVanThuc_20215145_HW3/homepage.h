#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <arpa/inet.h>
#include <netdb.h>

int isValidIP(char *ip)
{
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0;
}

int isValidDomain(const char *domain) {
    const char *domainPattern = "^([a-zA-Z0-9][-a-zA-Z0-9]*[a-zA-Z0-9]?\\.)+[a-zA-Z]{2,}$"; // Định dạng tên miền
    regex_t regex;
    int reti;

    reti = regcomp(&regex, domainPattern, REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        return 0;
    }

    reti = regexec(&regex, domain, 0, NULL, 0);
    regfree(&regex);
    return reti == 0;
}

void lookupByIP(char *ip)
{
    struct hostent *host;
    struct in_addr addr;

    if (!isValidIP(ip))
    {
        printf("Invalid IP address\n");
        return;
    }

    inet_pton(AF_INET, ip, &addr);
    host = gethostbyaddr(&addr, sizeof(addr), AF_INET);

    if (host == NULL)
    {
        printf("No information found for IP: %s\n", ip);
    }
    else
    {
        printf("Main name: %s\n", host->h_name);
        printf("Alternate names: ");
        for (char **alias = host->h_aliases; *alias != NULL; alias++)
        {
            printf("%s ", *alias);
        }
        printf("\n");
    }
}

void lookupByName(char *domain)
{
    struct hostent *host = gethostbyname(domain);

    if (host == NULL)
    {
        printf("No information found for domain: %s\n", domain);
    }
    else
    {
        printf("Main IP: %s\n", inet_ntoa(*(struct in_addr *)host->h_addr));
        printf("Alternate IPs: ");
        for (char **alias = host->h_aliases; *alias != NULL; alias++)
        {
            struct hostent *altHost = gethostbyname(*alias);
            if (altHost) {
                printf("%s ", inet_ntoa(*(struct in_addr *)altHost->h_addr));
            }
        }
        printf("\n");
    }
}
