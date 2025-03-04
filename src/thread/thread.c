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
            CONNECTION* current = c;
            c = c->next;

            if(conn_cconected(current) == 0x00) {
                
                if(current->prev != NULL) {
                    current->prev->next = current->next;
                }
                else {
                    ctx->pool->members[ctx->index].root = current->next;
                }

                if(current->next != NULL) {
                    current->next->prev = current->prev;
                }
                else {
                    ctx->pool->members[ctx->index].last = current->prev;
                }

                free(current);

                --ctx->pool->members[ctx->index].size;

                printf("Client disconnected! (%d in IO #%d)\n", ctx->pool->members[ctx->index].size, ctx->index);
                continue;
            }

            char buff[HEXASTRIKE_IO_BUFFER_SIZE];
            int r = recv(current->socket, buff, HEXASTRIKE_IO_BUFFER_SIZE, 0);

            if(r > 0) {
                ctx->r_handler(current, buff, r, ctx->index);            
            }
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