#include <server.h>
#include <client/client.h>
#include <stdio.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>
    #define closesocket(s) close(s)
#endif

void c_dsconn(HEXASTRIKE_SERVER* server, int index, CONNECTION* c) {
    closesocket(c->socket);

    if(c->prev != NULL) {
        c->prev->next = c->next;
    }
    else {
        server->pool.members[index].root = c->next;
    }

    free(c);
    --server->pool.members[index].size;

#ifdef HEXASTRIKE_DEBUG_LOGS
    printf("Client was disconnected (%d in IO #%d)\n", server->pool.members[index].size, index);
#endif
}
