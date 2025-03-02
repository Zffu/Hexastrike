#include <hexastrike.h>
#include <thread/pool.h>
#include <server.h>
#include <client/connection.h>
#include <debug.h>

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <WinSock2.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#endif

HEXASTRIKE_SERVER* hexastrike_sinit(short port) {

    HEXASTRIKE_SERVER* server = malloc(sizeof(HEXASTRIKE_SERVER));

    server->pool.indicator = ~(server->pool.indicator & 0); // Sets indicators to running

    for(int i = 0; i < HEXASTRIKE_IO_THREAD_POOL_MEMBERS; ++i) {
        server->pool.members[i].root = NULL;
        server->pool.members[i].last = NULL;
        server->pool.members[i].size = 0;
    }

    #ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
    #endif

    server->server_socket = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;

    setsockopt(server->server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*) &opt, sizeof(opt));

    #ifdef HEXASTRIKE_NULL_CHECKS
    if(server->server_socket < 0) {
        printf("Server socket creation failed!\n");
        return NULL;
    }
    #endif

    saddr server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    int i = bind(server->server_socket, (saddr_g*)&server_addr, sizeof(server_addr));

    #ifdef HEXASTRIKE_NULL_CHECKS
    if(i < 0) {
        printf("Server socket binding failed!\n");
    }
    #endif

    i = listen(server->server_socket, 5);

    #ifdef HEXASTRIKE_NULL_CHECKS
    if(i < 0) {
        printf("Server socket listening failed!\n");
    }
    #endif

    return server;
}

void hexastrike_sdown(HEXASTRIKE_SERVER* server) {
#ifdef _WIN32
    closesocket(server->server_socket);
    WSACleanup();
#else 
    close(server->server_socket);
#endif    

    // Free members
    for(int i = 0; i < HEXASTRIKE_IO_THREAD_POOL_MEMBERS; ++i) {
        if(server->pool.members[i].root == NULL) continue;

        CONNECTION* c = server->pool.members[i].root;
        
        while(c == NULL) {
            CONNECTION* n = c->next;

            free(c);
            c = n;
        }
    }

    free(server);
}

void hexastrike_dloop(HEXASTRIKE_SERVER* server) {
#ifndef HEXASTRIKE_NORUN_INDICATOR
    while(server->running) {
#else 
    while(1) {
#endif
        saddr caddr;
        int len = sizeof(caddr);

        socket_t csocket = accept(server->server_socket, (saddr_g*) &caddr, &len);

        if(csocket >= 0) {
            int low = INT_MAX;
            IO_THREADPOOL_MEMBER* m = NULL;

            for(int i = 0; i < HEXASTRIKE_IO_THREAD_POOL_MEMBERS; ++i) {
                if(server->pool.members[i].size < low) {
                    m = &server->pool.members[i];
                    low = m->size;
                }
            }

            if(m != NULL) {
                CONNECTION* c = malloc(sizeof(CONNECTION));
                
                c->address = caddr;
                c->socket = csocket;
                c->next = NULL;

                if(m->root == NULL) {
                    c->prev = NULL;
                    m->root = c;
                    m->last = c;
                }
                else {
                    c->prev = m->last;
                    m->last->next = c;
                    m->last = c;
                }
                ++m->size;

                printf("Added new connection to thread pool!, size: %d\n", m->size);
                
                printf("Approximate Total RAM usage: %db\n", dmem_usage(server));

            }
            #ifdef HEXASTRIKE_INFO_POOL_ASSOCIATION
            else {
                printf("Client association in thread pool failed!\n");
            }
            #endif

        }
    }
}