#include <thread/ctx.h>
#include <client/connection.h>

#include <stdlib.h>

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

    while(ctx->pool->indicator & (1L << ctx->index)) {
        if(ctx->pool->members[ctx->index].size == 0) {
            thread_sleep(5); // debug
            continue;
        }

        CONNECTION* c = ctx->pool->members[ctx->index].root;
        
        while(c != NULL) {
            if(conn_cconected(c) == 0x00) {
                printf("Client disconnected!\n");

                if(c->prev != NULL) {
                    c->prev->next = c->next;
                }
                else {
                    ctx->pool->members[ctx->index].root = c->next;
                }

                free(c);
                --ctx->pool->members[ctx->index].size;

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