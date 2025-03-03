/**
 * Hexastrike cross-platform handler & default options
 */

#ifdef _WIN32
#include <WinSock2.h>

typedef SOCKET socket_t;
typedef struct sockaddr_in saddr;
typedef struct sockaddr saddr_g;

#else 
#include <sys/socket.h>

typedef int socket_t;
typedef struct sockaddr_in saddr;
typedef struct sockaddr saddr_g;

#endif
