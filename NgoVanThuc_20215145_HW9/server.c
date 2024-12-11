// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include "account.h"

#define MAX_CLIENTS 1024
#define BUFFER_SIZE 1024

Account users[MAX_CLIENTS];
int user_count = 0;

Account *accountListHeader = NULL;

// Load user data from file
void load_users(char *filename)
{
    accountListHeader = readAccountFromFile(filename);
}

// Find user index by username
Account *find_user_is_login(const char *username, int client_id)
{
    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].username, username) == 0 && users[i].client_sock == client_id)
        {
            return &users[i];
        }
    }
    return NULL;
}

// Main server function
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    struct pollfd poll_fds[MAX_CLIENTS];
    int nfds = 1;

    load_users("nguoidung.txt");

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) == -1)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", port);

    poll_fds[0].fd = server_fd;
    poll_fds[0].events = POLLIN;

    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        poll_fds[i].fd = -1;
    }

    char buffer[BUFFER_SIZE];

    while (1)
    {
        int poll_count = poll(poll_fds, nfds, -1);
        if (poll_count == -1)
        {
            perror("Poll failed");
            break;
        }

        for (int i = 0; i < nfds; i++)
        {
            if (poll_fds[i].revents & POLLIN)
            {
                if (poll_fds[i].fd == server_fd)
                {
                    // Accept new connection
                    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
                    if (client_fd == -1)
                    {
                        perror("Accept failed");
                        continue;
                    }

                    printf("New client connected: %d\n", client_fd);

                    for (int j = 1; j < MAX_CLIENTS; j++)
                    {
                        if (poll_fds[j].fd == -1)
                        {
                            poll_fds[j].fd = client_fd;
                            poll_fds[j].events = POLLIN;
                            if (j >= nfds)
                            {
                                nfds = j + 1;
                            }
                            break;
                        }
                    }
                }
                else
                {
                    // Handle client message
                    memset(buffer, 0, BUFFER_SIZE);
                    int bytes_read = recv(poll_fds[i].fd, buffer, BUFFER_SIZE, 0);

                    if (bytes_read <= 0)
                    {
                        printf("Client %d disconnected\n", poll_fds[i].fd);
                        close(poll_fds[i].fd);
                        poll_fds[i].fd = -1;
                        continue;
                    }

                    printf("Received from client %d: %s\n", poll_fds[i].fd, buffer);

                    char *command = strtok(buffer, " ");
                    if (strcmp(command, "LOGIN") == 0)
                    {
                        char *username = strtok(NULL, " ");
                        char *password = strtok(NULL, " ");

                        Account *user_idx = findAccount(username);

                        if (user_idx == NULL || strcmp(user_idx->password, password) != 0 || user_idx->status != 1)
                        {
                            // TO DO STH
                            send(poll_fds[i].fd, "LOGIN FAILED\n", 13, 0);

                            if (user_idx == NULL)
                            {
                                send(poll_fds[i].fd, "USER NOT FOUND\n", 14, 0);
                            }
                            else if (strcmp(user_idx->password, password) != 0)
                            {
                                send(poll_fds[i].fd, "WRONG PASSWORD\n", 15, 0);
                                user_idx->loginError++;
                                updateAccountFile(user_idx);
                                if(user_idx->loginError >= 3)
                                {
                                    user_idx->status = 0;
                                    updateAccountFile(user_idx);
                                    send(poll_fds[i].fd, "ACCOUNT BLOCKED\n", 16, 0);
                                }
                            }
                            else if (user_idx->status != 1)
                            {
                                send(poll_fds[i].fd, "ACCOUNT BLOCKED\n", 16, 0);
                            }

                        }
                        else
                        {
                            user_idx->client_sock = poll_fds[i].fd;
                            users[user_count++] = *user_idx;
                            user_idx->loginError = 0;
                            updateAccountFile(user_idx);
                            send(poll_fds[i].fd, "LOGIN SUCCESS\n", 14, 0);
                        }
                    }
                    // Handle other commands like CHANGE_PASSWORD, LOGOUT...
                    else if (strcmp(command, "CHANGE_PASSWORD") == 0)
                    {
                        // Handle change password
                        char *username = strtok(NULL, " ");
                        char *new_password = strtok(NULL, " ");

                        Account *user = find_user_is_login(username, poll_fds[i].fd);

                        if (user == NULL)
                        {
                            send(poll_fds[i].fd, "USER NOT FOUND OR YOU ARE NOT LOGGED IN\n", 40, 0);
                        }
                        else
                        {

                            strcpy(user->password, new_password);
                            updateAccountPassword(username, new_password);

                            // send(poll_fds[i].fd, "PASSWORD CHANGED\n", 17, 0);

                            for (int i = 0; i < user_count; i++)
                            {
                                if (strcmp(users[i].username, username) == 0)
                                {
                                    send(users[i].client_sock, "PASSWORD CHANGED\n", 17, 0);
                                }
                            }
                        }
                    }
                    else if (strcmp(command, "LOGOUT") == 0)
                    {
                        // Handle logout

                        char *username = strtok(NULL, " ");
                        Account *user = find_user_is_login(username, poll_fds[i].fd);

                        if (user == NULL)
                        {
                            send(poll_fds[i].fd, "USER NOT FOUND OR YOU ARE NOT LOGGED IN\n", 40, 0);
                        }

                        for (int j = 0; j < user_count; j++)
                        {
                            if (users[j].client_sock == user->client_sock && strcmp(users[j].username, username) == 0)
                            {
                                users[i].client_sock = -1;
                                send(poll_fds[i].fd, "LOGOUT SUCCESS\n", 15, 0);

                                users[i] = users[user_count - 1];
                                user_count--;
                                break;
                            }
                        }

                        // send(poll_fds[i].fd, "LOGOUT ERROR\n", 15, 0);


                    }
                    else {
                        send(poll_fds[i].fd, "INVALID COMMAND\n", 16, 0);
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
