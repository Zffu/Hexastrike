#include <thread/pool.h>

#ifndef THREAD_H 
#define THREAD_H

#ifdef _WIN32
unsigned __stdcall hexastrike_io_thread_pool_member_exec(void* arg); // HEXASTIRKE_IOPOOL_MEMBER_EXECCTX
#else 
void* hexastrike_io_thread_pool_member_exec(void* arg); // HEXASTIRKE_IOPOOL_MEMBER_EXECCTX
#endif

#endif