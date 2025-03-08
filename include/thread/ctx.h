#include <thread/pool.h>
#include <server.h>

#ifndef THREAD_CTX_H
#define THREAD_CTX_H

// Indicator for index & thread pool for thread pool member functions
typedef struct HEXASTRIKE_IO_THREAD_POOL_MEMBER_EXECUTION_CONTEXT {

    IO_THREAD_POOL* pool;

    HEXASTRIKE_SERVER* serverPtr;
    
    int index;

} IOPOOL_MEMBER_EXECCTX;

#endif