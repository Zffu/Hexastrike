#include <client/connection.h>

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#ifdef HEXASTIKE_DISABLE_CONN_TREE // Not recomended at all (not implemented rn)

typedef struct {

    CONNECTION* connections;
    int size;
    int allocatedSize;

} IO_THREADPOOL_MEMBER;

#else 

typedef struct {

    CONNECTION* root;
    CONNECTION* last;
    int size;

} IO_THREADPOOL_MEMBER;

#endif


// Hexastrike IO Thread pooling
typedef struct HEXASTRIKE_IO_THREAD_POOL {

    IO_THREADPOOL_MEMBER members[HEXASTRIKE_IO_THREAD_POOL_MEMBERS];
    HEXASTIRKE_IOTHREAD_INDICATOR indicator; // bit -> thread preserve storage

} IO_THREAD_POOL;

#endif