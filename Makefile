C_SOURCES = $(wildcard src/*.c src/**/*.c src/**/**/*.c)
HEADERS = $(wildcard include/*.h include/*/*.h include/*/*/*.h)

# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o}

# -g: Use debugging symbols in gcc
DEBUG_FLAGS = -g -DHEXASTRIKE_NULL_CHECKS=1
FLAGS = -DHEXASTRIKE_IO_THREAD_POOL_MEMBERS=2 -DHEXASTRIKE_IO_BUFFER_SIZE=2048 -DHEXASTIRKE_IOTHREAD_INDICATOR="unsigned char"

CFLAGS = -g -Iinclude -lws2_32 $(FLAGS)

TARGET = hexastrike.exe

all: $(TARGET)

$(TARGET): ${C_SOURCES}
	gcc ${C_SOURCES} -o hexastrike.exe ${CFLAGS}