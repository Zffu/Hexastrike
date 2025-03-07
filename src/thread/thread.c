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

#ifndef HEXASTIRKE_NO_D_HANDLER
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
    ctx->serverPtr->d_handler(c, ctx->index);

#else
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
    --ctx->pool->members[ctx->index].size; 

#endif

#ifdef _WIN32
unsigned __stdcall hexastrike_io_thread_pool_member_exec(void* arg) { 
#else 
void* hexastrike_io_thread_pool_member_exec(void* arg) {
#endif
    IOPOOL_MEMBER_EXECCTX* ctx = (IOPOOL_MEMBER_EXECCTX*) arg;

#ifdef HEXASTRIKE_SOFT_IO_THREAD
    int empty_iter = 0;
#endif
    while(ctx->pool->indicator & (1L << ctx->index)) {
        if(ctx->pool->members[ctx->index].size == 0) {
#ifdef HEXASTRIKE_SOFT_IO_THREAD
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

            int r = recv(curr->socket, buff, HEXASTRIKE_IO_BUFFER_SIZE, 0);

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

#ifndef HEXASTIRKE_NO_R_HANDLER
            ctx->serverPtr->r_handler(curr, buff, r, ctx->index);
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