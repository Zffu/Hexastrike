#include <hexastrike.h>
#include <server.h>
#include <client/connection.h>
#include <client/client.h>

HEXASTRIKE_SERVER* server;

void handle(CONNECTION* conn, unsigned char* buff, int size, int index) {
    c_dsconn(server, index, conn);
}

int main() {
    server = hexastrike_sinit(3000);

    hexastrike_iopinit(server);
    hexastrike_dloop(server);
}