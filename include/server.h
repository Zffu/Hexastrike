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

    void (*r_handler)(CONNECTION, unsigned char*, int);

} HEXASTRIKE_SERVER;

HEXASTRIKE_SERVER* hexastrike_sinit(short port);

void hexastrike_sdown(HEXASTRIKE_SERVER* server);

void hexastrike_dloop(HEXASTRIKE_SERVER* server);
void hexastrike_iopinit(HEXASTRIKE_SERVER* server);

#endif