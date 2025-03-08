#include <hexastrike.h>

#include <thread/ctx.h>
#include <thread/thread.h>
#include <client/connection.h>

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <process.h>
#else 
#include <pthread.h>
#endif

#ifndef HEXASTRIKE_FASTMOD
#endif


#define THREAD_DISCONNECT(ctx, c) \
    if(c->prev != NULL) { \
        c->prev->next = c->next; \
    } \
    else { \
        ctx->pool->members[ctx->index].root = c->next; \
    } \
    \
    if(c->next != NULL) { \
        c->next->prev = c->prev; \
    } \
    else { \
        ctx->pool->members[ctx->index].last = c->prev; \
    } \
    \
    free(c); \
    --ctx->pool->members[ctx->index].size; \
    if(ctx->serverPtr->flags & F_HANDLE_DISCONNECT) ctx->serverPtr->d_handler(c, ctx->index);

#ifdef _WIN32
unsigned __stdcall hexastrike_io_thread_pool_member_exec(void* arg) { 
#else 
void* hexastrike_io_thread_pool_member_exec(void* arg) {
#endif
    IOPOOL_MEMBER_EXECCTX* ctx = (IOPOOL_MEMBER_EXECCTX*) arg;

#if defined(HEXASTRIKE_SOFT_IO_THREAD)
    int empty_iter = 0;
#endif

    while(ctx->pool->indicator & (1L << ctx->index)) {
        if(ctx->pool->members[ctx->index].size == 0) {
#if defined(HEXASTRIKE_SOFT_IO_THREAD)
            ++empty_iter;

            if(empty_iter >= 1000) {
                thread_sleep(100);
            }
            else if(empty_iter >= 100) {
                thread_sleep(10);
            }
            else if(empty_iter >= 10) {
                thread_sleep(1);
            }
#endif
            continue;
        }
#ifdef HEXASTRIKE_SOFT_IO_THREAD
        empty_iter = 0;
#endif

        CONNECTION* c = ctx->pool->members[ctx->index].root;

        while (c != NULL) {
            CONNECTION* curr = c;
            c = c->next;
            unsigned char buff[HEXASTRIKE_IO_BUFFER_SIZE] = {0};

            int r = recv(curr->socket, buff, HEXASTIRKE_IO_BUFFER_SIZE, (ctx->serverPtr->flags & F_PEEK_BUFF_SIZE) ? MSG_PEEK : 0);

            if(r == 0) {
                THREAD_DISCONNECT(ctx, curr);
                continue;
            }
            else if(r < 0) {
                #ifdef _WIN32
                int e = WSAGetLastError();
                if(e == WSAECONNRESET || e == WSAECONNABORTED) {
                    THREAD_DISCONNECT(ctx, curr);
                    continue;
                }
                #else
                if (errno == ECONNRESET || errno == ECONNABORTED) {
                    THREAD_DISCONNECT(ctx, curr);
                    continue;
                }
                #endif
            }

            if(ctx->serverPtr->flags & F_PEEK_BUFF_SIZE) {
                int sz = ctx->serverPtr->size_determinator(buff, r);
                free(buff);
                buff = malloc(sz);

                r = recv(curr->socket, buff, sz, 0);
            }

            ctx->serverPtr->r_handler(curr, buff, r, ctx->index);
        }
    }
}

#ifdef _WIN32
unsigned __stdcall hexastrike_dloop_thread_exec(void* arg) {
#else
void* hexastrike_dloop_thread_exec(void* arg) {
#endif

    HEXASTRIKE_SERVER* server = (HEXASTRIKE_SERVER*) arg;
#ifdef HEXASTRIKE_SOFT_IO_THREAD
    int empty_iter = 0;
#endif

    while(server->running) {
        saddr caddr;
        int len = sizeof(caddr);

        socket_t csocket = accept(server->server_socket, (saddr_g*) &caddr, &len);

#ifdef HEXASTRIKE_SOFT_IO_THREAD
        if(csocket == INVALID_SOCKET) {
            ++empty_iter;

            if(empty_iter >= 1000) {
                thread_sleep(100);
            }
            else if(empty_iter >= 100) {
                thread_sleep(10);
            }
            else if(empty_iter >= 10) {
                thread_sleep(1);
            }
            continue;
        } 
        else {
            empty_iter = 0;
#else
        if(csocket != INVALID_SOCKET) {
#endif
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
                continue;
            }
#endif
        }
    }
}




void thread_sleep(long time) {
#ifdef _WIN32  
    Sleep(time);
#else
    usleep(time * 1000);
#endif
}