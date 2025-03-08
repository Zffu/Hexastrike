#include <hexastrike.h>
#include <server.h>
#include <stdio.h>
#include <client/connection.h>
#include <stdlib.h>
#include <stdio.h>

void disc(CONNECTION* c, int index) {
    printf("Bai\n");
}

int main() {
    HEXASTRIKE_SERVER* server = hexastrike_sinit(3000);

    server->flags |= F_HANDLE_DISCONNECT;

    server->d_handler = disc;

    hexastrike_iopinit(server);
    hexastrike_dloop(server);

    printf("Test");
}