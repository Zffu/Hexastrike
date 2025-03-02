#include <hexastrike.h>
#include <server.h>

int main() {
    HEXASTRIKE_SERVER* server = hexastrike_sinit(3000);

    hexastrike_dloop(server);
    
    hexastrike_sdown(server);
}