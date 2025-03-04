#include <thread/pool.h>

#ifndef THREAD_CTX_H
#define THREAD_CTX_H

// Indicator for index & thread pool for thread pool member functions
typedef struct HEXASTRIKE_IO_THREAD_POOL_MEMBER_EXECUTION_CONTEXT {

    IO_THREAD_POOL* pool;

#ifndef HEXASTIRKE_NO_R_HANDLER
    void (*r_handler)(CONNECTION*, unsigned char*, int, int); //todo: change this for direct link after
#endif

#ifndef HEXASTIRKE_NO_D_HANDLER
    void (*d_handler)(CONNECTION*, int);
#endif
    int index;

} IOPOOL_MEMBER_EXECCTX;

#endif