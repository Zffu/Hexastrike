#include <hexastrike.h>
#include <thread/pool.h>

#include <client/connection.h>

#define ERRSOCKETINIT (-1 << 0)
#define ERRSOCKETBIND (-1 << 1)
#define ERRSOCKETLISTEN (-1 << 2)
#define ERRPOOLASSOCIATION (-1 << 3)
#define ERRIOTHREADINIT (-1 << 4)

#ifndef SERVER_H
#define SERVER_H

// Hexastrike server root struct
typedef struct HEXASTRIKE_SOCKET_SERVER {

    unsigned char running;

    socket_t server_socket;
    IO_THREAD_POOL pool;

    long flags;

    void (*r_handler)(CONNECTION*, unsigned char*, int, int);
    void (*d_handler)(CONNECTION*, int);

    int (*size_determinator)(unsigned char*, int);

} HEXASTRIKE_SERVER;

HEXASTRIKE_SERVER* hexastrike_sinit(short port);

void hexastrike_sdown(HEXASTRIKE_SERVER* server);

#ifndef HEXASTRIKE_NULL_CHECKS
void hexastrike_dloop(HEXASTRIKE_SERVER* server);
#else 
unsigned char hexastrike_dloop(HEXASTRIKE_SERVER* server);
#endif

#ifndef HEXASTRIKE_NULL_CHECKS
void hexastrike_iopinit(HEXASTRIKE_SERVER* server);
#else
unsigned char hexastrike_iopinit(HEXASTRIKE_SERVER* server);
#endif

void hexastrike_dloop_tinit(HEXASTRIKE_SERVER* server);

#endif