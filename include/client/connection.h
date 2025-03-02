#include <hexastrike.h>

#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

// Hexastrike connection (tree disabled)
struct CONNECTION_NOTREE {

    socket_t socket;
    saddr address;

};

// Hexastrike connection (tree)
struct CONNECTION_TREE {

    socket_t socket;
    saddr address;

    struct CONNECTION_TREE* prev;
    struct CONNECTION_TREE* next;

};

#ifdef HEXASTIKE_DISABLE_CONN_TREE
typedef struct CONNECTION_NOTREE CONNECTION; // Not recomended at all! 
#else 
typedef struct CONNECTION_TREE CONNECTION; 
#endif

unsigned char conn_cconected(CONNECTION* c);

#endif