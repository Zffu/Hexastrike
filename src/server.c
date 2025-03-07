#include <hexastrike.h>

#include <thread/pool.h>
#include <thread/thread.h>
#include <thread/ctx.h>

#include <server.h>
#include <client/connection.h>

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <WinSock2.h>
#include <process.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

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

    #ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(server->server_socket, FIONBIO, &mode);
    #else
    int flags = fcntl(server->server_socket, F_GETFL, 0);
    fcntl(server->server_socket, F_SETFL, flags | O_NONBLOCK);
    #endif

    #ifdef HEXASTRIKE_NULL_CHECKS
    if(server->server_socket < 0) {
        return (void*) ERRSOCKETINIT;
    }
    #endif

    saddr server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    int i = bind(server->server_socket, (saddr_g*)&server_addr, sizeof(server_addr));

    #ifdef HEXASTRIKE_NULL_CHECKS
    if(i < 0) {
        return (void*) ERRSOCKETBIND;
    }
    #endif

    i = listen(server->server_socket, 128);

    #ifdef HEXASTRIKE_NULL_CHECKS
    if(i < 0) {
        return (void*) ERRSOCKETLISTEN;
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
        
        while(c != NULL) {
            CONNECTION* n = c->next;

            free(c);
            c = n;
        }
    }

    free(server);
}

#ifndef HEXASTRIKE_NULL_CHECKS
void hexastrike_dloop(HEXASTRIKE_SERVER* server) {
#else
unsigned char hexastrike_dloop(HEXASTRIKE_SERVER* server) {
#endif

#ifndef HEXASTRIKE_NORUN_INDICATOR
    while(server->running) {
#else 
    while(1) {
#endif
        saddr caddr;
        int len = sizeof(caddr);

        socket_t csocket = accept(server->server_socket, (saddr_g*) &caddr, &len);

        if(csocket != INVALID_SOCKET) {
            int low = INT_MAX;
            IO_THREADPOOL_MEMBER* m = NULL;

            for(int i = 0; i < HEXASTRIKE_IO_THREAD_POOL_MEMBERS; ++i) {
                if(server->pool.members[i].size < low) {
                    m = &server->pool.members[i];
                    low = m->size;
                }
            }

            if(m != NULL) {
#ifndef HEXASTRIKE_CONN_ALLOCSIZE
                CONNECTION* c = malloc(sizeof(CONNECTION));
#else
                CONNECTION* c = malloc(HEXASTRIKE_CONN_ALLOCSIZE);
#endif
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
                
            }
#ifdef HEXASTRIKE_NULL_CHECKS
            else {
                return ERRPOOLASSOCIATION;
            }
#endif

        }
    }
#ifdef HEXASTRIKE_NULL_CHECKS
    return 0x01;
#endif
}

#ifndef HEXASTRIKE_NULL_CHECKS
void hexastrike_iopinit(HEXASTRIKE_SERVER* server) {
#else
unsigned char hexastrike_iopinit(HEXASTRIKE_SERVER* server) {
#endif
    for(int i = 0; i < HEXASTRIKE_IO_THREAD_POOL_MEMBERS; ++i) {

        IOPOOL_MEMBER_EXECCTX* ctx = malloc(sizeof(IOPOOL_MEMBER_EXECCTX));
        ctx->index = i;
#if !defined (HEXASTIRKE_NO_D_HANDLER) || !defined(HEXASTIRKE_NO_R_HANDLER)
        ctx->serverPtr = server;
#endif
        ctx->pool = &server->pool;

#ifdef _WIN32
        HANDLE thread = (HANDLE) _beginthreadex(NULL, 0, hexastrike_io_thread_pool_member_exec, ctx, 0, NULL);
    
#ifdef HEXASTRIKE_NULL_CHECKS
        if(thread == NULL) {
            free(ctx);
            return ERRIOTHREADINIT;
        }
#endif

        CloseHandle(thread);

#else
        pthread_t t;
        int i = pthread_create(&t, NULL, hexastrike_io_thread_pool_member_exec, ctx);

#ifdef HEXASTRIKE_NULL_CHECKS
        if(i != 0) {
            free(ctx);
            return ERRIOTHREADINIT;
        }
#endif

        pthread_detach(t);

#endif
    }
#ifdef HEXASTRIKE_NULL_CHECKS
    return 0x01;
#endif
}

void hexastrike_dloop_tinit(HEXASTRIKE_SERVER* server) {
#ifdef _WIN32
    HANDLE thread = (HANDLE) _beginthreadex(NULL, 0, hexastrike_dloop_thread_exec, server, 0, NULL);
    CloseHandle(thread);

#else
    pthread_t t;
    int i = pthread_create(&t, NULL, hexastrike_dloop_thread_exec, server);
    pthread_detach(t);
#endif
}