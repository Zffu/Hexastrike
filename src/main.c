#include <hexastrike.h>
#include <server.h>
#include <client/connection.h>

void handle(CONNECTION* conn, unsigned char* buff, int size) {
    
}

int main() {
    HEXASTRIKE_SERVER* server = hexastrike_sinit(3000);

    hexastrike_iopinit(server);
    hexastrike_dloop(server);
}