#include <thread/pool.h>
#include <server.h>

#ifndef THREAD_CTX_H
#define THREAD_CTX_H

// Indicator for index & thread pool for thread pool member functions
typedef struct HEXASTRIKE_IO_THREAD_POOL_MEMBER_EXECUTION_CONTEXT {

    IO_THREAD_POOL* pool;

#if !defined (HEXASTRIKE_NO_D_HANDLER) || !defined(HEXASTRIKE_NO_R_HANDLER) || !defined(HEXASTRIKE_FASTMODE)
    HEXASTRIKE_SERVER* serverPtr;
#endif

    int index;

} IOPOOL_MEMBER_EXECCTX;

#endif