#include <hexastrike.h>
#include <server.h>
#include <stdio.h>

int main() {
    HEXASTRIKE_SERVER* server = hexastrike_sinit(3000);
    hexastrike_iopinit(server);
    hexastrike_dloop_tinit(server);

    printf("Test");
}