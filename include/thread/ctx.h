#include <thread/pool.h>

#ifndef THREAD_CTX_H
#define THREAD_CTX_H

// Indicator for index & thread pool for thread pool member functions
typedef struct HEXASTRIKE_IO_THREAD_POOL_MEMBER_EXECUTION_CONTEXT {

    IO_THREAD_POOL* pool;
    void (*r_handler)(CONNECTION*, unsigned char*, int, int); //todo: change this for direct link after
    int index;

} IOPOOL_MEMBER_EXECCTX;

#endif