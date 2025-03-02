#include <server.h>
#include <hexastrike.h>

int dmem_usage(HEXASTRIKE_SERVER* server) {
    int sz = sizeof(*server);

    for(int i = 0; i < HEXASTRIKE_IO_THREAD_POOL_MEMBERS; ++i) {
        if(server->pool.members[i].size == 0) continue;
        
        CONNECTION* c = server->pool.members[i].root;

        while(c != NULL) {
        
            sz += sizeof(*c);
            c = c->next;
        }
    }
    
    return sz;
}