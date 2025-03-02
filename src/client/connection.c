#include <client/connection.h>

#ifdef _WIN32
#include <WinSock2.h>
#else 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#endif

unsigned char conn_cconected(CONNECTION* c) {
    char buff;
    int r;

#ifdef _WIN32
    r = recv(c->socket, &buff, 1, MSG_PEEK);
    if(r == 0) return 0;
    if(r == -1) {
        if(WSAGetLastError() == WSAEWOULDBLOCK) return 1;
        return 0;
    }
#else
    r = recv(c->socket, &buff, 1, MSG_PEEK | MSG_DONTWAIT);
    if(r == 0) return 0;
    if(r == -1) {
        if(errno == EAGAIN || errno == EWOULDBLOCK) return 1;
        return 0;
    }
#endif

    return 1;
}