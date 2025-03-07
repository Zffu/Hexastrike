#include <hexastrike.h>
#include <thread/pool.h>

#include <client/connection.h>

#ifndef SERVER_H
#define SERVER_H

// Hexastrike server root struct
typedef struct HEXASTRIKE_SOCKET_SERVER {

    #ifndef HEXASTRIKE_NORUN_INDICATOR
    unsigned char running;
    #endif

    socket_t server_socket;
    IO_THREAD_POOL pool;

#ifndef HEXASTIRKE_NO_R_HANDLER
    void (*r_handler)(CONNECTION*, unsigned char*, int, int);
#endif

#ifndef HEXASTRIKE_NO_D_HANDLER
    void (*d_handler)(CONNECTION*, int);
#endif

} HEXASTRIKE_SERVER;

// Hexastrike flag server root struct
typedef struct HEXASTIRKE_FLAG_SOCKET_SERVER {

#ifndef HEXASTRIKE_NORUN_INDICATOR
    unsigned char running;
#endif

    socket_t server_socket;
    IO_THREAD_POOL pool;

    void (*r_handler)(CONNECTION*, unsigned char*, int, int);
    void (*d_handler)(CONNECTION*, int);

#ifdef HEXASTRIKE_NORUN_INDICATOR
    unsigned char running; // ptr conversion offset
#endif

    int flags;

    int ioPoolThreads;
    int ioBufferSize;
    int connectionAllocSize;
} HEXASTRIKE_F_SERVER;


HEXASTRIKE_SERVER* hexastrike_sinit(short port);
HEXASTRIKE_F_SERVER* hexastrike_fsinit(short port);

void hexastrike_sdown(HEXASTRIKE_SERVER* server);
void hexastrike_fsdown(HEXASTRIKE_F_SERVER* server);

unsigned char hexastrike_fdloop(HEXASTRIKE_F_SERVER* server);
unsigned char hexastrike_fiopinit(HEXASTRIKE_F_SERVER* server);

#ifndef HEXASTRIKE_NULL_CHECKS
void hexastrike_dloop(HEXASTRIKE_SERVER* server);
#else 
unsigned char hexastrike_dloop(HEXASTRIKE_SERVER);
#endif

#ifndef HEXASTRIKE_NULL_CHECKS
void hexastrike_iopinit(HEXASTRIKE_SERVER* server);
#else
unsigned char hexastrike_iopinit(HEXASTRIKE_SERVER* server);
#endif

#endif