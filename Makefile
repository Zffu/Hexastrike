C_SOURCES = $(wildcard src/*.c src/**/*.c src/**/**/*.c testserver/*.c)
HEADERS = $(wildcard include/*.h include/*/*.h include/*/*/*.h)

# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o}

# Hexastrike Flags
HEXASTRIKE_DEBUG_FLAGS = -DHEXASTRIKE_NULL_CHECKS=1 -DHEXASTRIKE_DEBUG_LOGS=1

HEXASTIRKE_FLAGS = -DHEXASTRIKE_IO_THREAD_POOL_MEMBERS=2 -DHEXASTRIKE_IO_BUFFER_SIZE=2048 -DHEXASTIRKE_IOTHREAD_INDICATOR="unsigned char" -DHEXASTRIKE_SOFT_IO_THREAD=1 

FLAGS = -Iinclude -lws2_32 $(HEXASTIRKE_FLAGS)

DEBUG_FLAGS = $(FLAGS) -g -gdwarf-2 -O0 $(HEXASTRIKE_DEBUG_FLAGS)

TARGET = hexastrike.exe

all: $(TARGET)

$(TARGET): ${C_SOURCES}
	gcc ${C_SOURCES} -o hexastrike.exe ${DEBUG_FLAGS}