#include <thread/pool.h>

#ifndef THREAD_H 
#define THREAD_H

#ifdef _WIN32
unsigned __stdcall hexastrike_io_thread_pool_member_exec(void* arg); // HEXASTRIKE_IOPOOL_MEMBER_EXECCTX
#else 
void* hexastrike_io_thread_pool_member_exec(void* arg); // HEXASTRIKE_IOPOOL_MEMBER_EXECCTX
#endif

#ifdef _WIN32
unsigned __stdcall hexastrike_dloop_thread_exec(void* arg); // HEXASTRIKE_SERVER
#else
void* hexastrike_dloop_thread_exec(void* arg); // HEXASTRIKE_SERVER
#endif

void thread_sleep(long time);

#endif