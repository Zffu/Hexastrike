# Hexastrike
![GitHub Release](https://img.shields.io/github/v/release/Zffu/Hexastrike?include_prereleases)


Hexastrike is a **simplicity and efficiency driven** *Socket server* framework written in *C*.

## Why Hexastrike

Unlike most *Socket server* frameworks, **Hexastrike** prioritizes performance and ease of use over complexity. 

## Building

In order to build Hexastrike, you will need a C compiler and Make.
*(If you do not use GCC, you will have to update the Makefile)*

Then run this command:
```
make clean lib
```

This will generate a built library of Hexastrike that you will be able to use.

Alternatively, you can build a test version of Hexastrike with sample code by doing:
```
make clean all
```

### Build Options
Here's the list of building options that you can add in the Makefile currently:
- `-DHEXASTRIKE_NULL_CHECKS`: If this option is added, Hexastrike will have null checks

- `-DHEXASTRIKE_IO_THREAD_POOL_MEMBERS` (Required): Determines the amounts of IO working threads that Hexastrike will have
- `DHEXASTRIKE_IOTHREAD_INDICATOR` (Required): Determines the type of number that Hexastrike will use to store Thread active statuses. Must be a type that contains enough bits for all of the thread pool members to have a bit each.

- `-DHEXASTRIKE_IO_BUFFER_SIZE` (Required): The size of the IO byte buffer that Hexastrike will process.

- `-DHEXASTRIKE_SOFT_IO_THREAD`: Makes the IO threads slower and softer when no client is connected.

- `-DHEXASTRIKE_CONN_ALLOCSIZE`: Determines how much bytes will be allocated to each connection.

## Usage

### Getting Started

1. In order to use Hexastrike, you will first have to create an ``HEXASTRIKE_SERVER`` instance by doing the following:

    ```C
    #include <hexastrike/server.h>

    #define PORT 8080

    int main() {
        HEXASTRIKE_SERVER* server = hexastrike_sinit(PORT);
    }
    ```



2. After this we need to initialize the IO thread pool by doing the following:
    ```C
    hexastrike_iopinit(server);
    ```

    This will initialize the `IO Processing Thread Pool` and start up the threads.


3. Finally, we need to initialize the `IO Discovery` / `IO Listening` by doing the following:

    ```C
    hexastrike_dloop(server);
    ```

    Alternatively, if you want to keep the main thread and use another for `IO Discovery` / `IO Listening`, you can do this instead:
    
    ```C
    hexastrike_dloop_tinit(server);
    ```

### Adding Handlers

Instead of using an event loop like `epoll` for instance, Hexastrike uses a function driven alternative:

```C
#include <client/connection.h>
#include <client/client.h>

void handle_input(CONNECTION* conn, unsigned char* buff, int size, int index) {
    printf("[IO #%d] Client sent %d bytes!\n", index, size);
    c_dsconn(server, index, conn); // server represents your HEXASTRIKE_SERVER instance.
}

int main() {
    // Existing server code...
    server.r_handler = handle_input; 
}
```

This will register the `handle_input` function as the handler `r_handler` which handles recieved client data.

In this simple example, we notify how much bytes were recieved and disconnect the client after by using `c_dsconn`

**Note:** The `r_handler` is the only handler enabled by default, to enable the other handlers, you must add the flags for the specific handler.

Here's an example on how to enable the disconnect handler (`d_handler`):
```C
#include <hexastrike.h>

int main() {
    // Existing server code...
    server->flags |= F_HANDLE_DISCONNECT;
}
```

### Storing data for connections

Hexastrike allows to directly store data for each connection, in order to do so you must specify the `HEXASTRIKE_CONN_ALLOCSIZE` option, this option will determine how much size is allocated to the connection. (Please note that this value must be higher than 40).

As an example, let's say we want to store 5 bytes in the connection, we first need to set `HEXASTRIKE_CONN_ALLOCSIZE` to 45 (40 being the base CONNECTION size and 5 being the size of our data).

Then, in order to modify or get said data we can simply make a type similar to the connection but with our data: 

```C
#include <hexastrike.h>

typedef struct MY_CUSTOM_CONNECTION {

    socket_t socket;
    saddr address;

    struct MY_CUSTOM_CONNECTION* prev;
    struct MY_CUSTOM_CONNECTION* next;

    unsigned char myBytes[5];


} MY_CUSTOM_CONNECTION;
```

We can then simply cast the `CONNECTION` pointer to our `MY_CUSTOM_CONNECTION`:
```C
void handle(CONNECTION* c, unsigned char* b, int size, int index) {
    MY_CUSTOM_CONNECTION* my = (MY_CUSTOM_CONNECTION*) c;

    byte myByte = myBytes[3];
}
```

**Note:** Hexastrike will only free the original pointer, if you add any pointers to your `CONNECTION` structure you will need to free them.