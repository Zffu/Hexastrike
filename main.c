#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BACKLOG 10

void set_nonblocking(SOCKET sockfd) {
    u_long mode = 1;
    if (ioctlsocket(sockfd, FIONBIO, &mode) != 0) {
        perror("ioctlsocket");
        exit(EXIT_FAILURE);
    }
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    set_nonblocking(server_fd);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return EXIT_FAILURE;
    }

    if (listen(server_fd, BACKLOG) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Server listening on port %d (non-blocking mode)\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == INVALID_SOCKET) {
            int err = WSAGetLastError();
            if (err != WSAEWOULDBLOCK) {
                printf("Accept failed: %d\n", err);
            }
        } else {
            printf("New client connected!\n");
            closesocket(client_fd); // Close immediately for this minimal example
        }
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
