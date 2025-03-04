#include <hexastrike.h>
#include <server.h>

int main() {
    HEXASTRIKE_SERVER* server = hexastrike_sinit(3000);
    hexastrike_iopinit(server);
    hexastrike_dloop(server);
}