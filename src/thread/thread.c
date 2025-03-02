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

#ifdef _WIN32
unsigned __stdcall hexastrike_io_thread_pool_member_exec(void* arg) { 
#else 
void* hexastrike_io_thread_pool_member_exec(void* arg) {
#endif
    IOPOOL_MEMBER_EXECCTX* ctx = (IOPOOL_MEMBER_EXECCTX*) arg;

    int empty_iter = 0;
    while(ctx->pool->indicator & (1L << ctx->index)) {
        if(ctx->pool->members[ctx->index].size == 0) {
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
        empty_iter = 0;

        CONNECTION* c = ctx->pool->members[ctx->index].root;

        while(c != NULL) {

            if(conn_cconected(c) == 0x00) {
                CONNECTION* next = c->next;

                if(c->prev != NULL) {
                    c->prev->next = next;
                }
                else {
                    ctx->pool->members[ctx->index].root = next;
                }

                c = next;
                --ctx->pool->members[ctx->index].size;

                printf("Client disconnected! (%d in IO #%d)\n", ctx->pool->members[ctx->index].size, ctx->index);
                continue;
            }

            char buff[HEXASTRIKE_IO_BUFFER_SIZE];
            int r = recv(c->socket, buff, HEXASTRIKE_IO_BUFFER_SIZE, 0);

            if(r > 0) {
                printf("[IO #%d] Recieved %d bytes from client!\n", ctx->index, r);
            }
        
            c = c->next;
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