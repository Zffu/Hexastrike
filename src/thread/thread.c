#include <thread/ctx.h>
#include <client/connection.h>

#ifdef _WIN32
unsigned __stdcall hexastrike_io_thread_pool_member_exec(void* arg) { 
#else 
void* hexastrike_io_thread_pool_member_exec(void* arg) {
#endif
    IOPOOL_MEMBER_EXECCTX* ctx = (IOPOOL_MEMBER_EXECCTX*) arg;

    while(ctx->pool->indicator & (1L << ctx->index)) {

        CONNECTION* c = ctx->pool->members->root;
        
        while(c != NULL) {
            
            //todo: after

            c = c->next;
        }

    }

}