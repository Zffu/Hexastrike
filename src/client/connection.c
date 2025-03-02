#include <client/connection.h>

#include <stdlib.h>
#include <stdio.h>

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
    char buffer[1];
    int bytes_received = recv(c->socket, buffer, sizeof(buffer), MSG_PEEK);
    
    if (bytes_received == 0) {
        return 0x00; // Connection closed gracefully
    } else if (bytes_received < 0) {
        #ifdef _WIN32
        int error = WSAGetLastError();
        if (error == WSAECONNRESET || error == WSAECONNABORTED) {
            return 0x00;
        }
        #else
        if (errno == ECONNRESET || errno == ECONNABORTED) {
            return 0x00;
        }
        #endif
    }
    return 0x01;
}