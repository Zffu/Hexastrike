#include <hexastrike.h>
#include <server.h>
#include <client/connection.h>
#include <client/client.h>

#include <stdlib.h>
#include <stdio.h>

HEXASTRIKE_SERVER* server;

void handle(CONNECTION* conn, unsigned char* buff, int size, int index) {
    printf("[IO #%d] Client sent %d bytes!\n", index, size);
    c_dsconn(server, index, conn);
}

int main() {
    server = hexastrike_sinit(3000);

    server->r_handler = handle;

    hexastrike_iopinit(server);
    hexastrike_dloop(server);
}