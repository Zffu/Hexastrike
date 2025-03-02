C_SOURCES = $(wildcard src/*.c src/**/*.c src/**/**/*.c)
HEADERS = $(wildcard include/*.h include/*/*.h include/*/*/*.h)

# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o}

# -g: Use debugging symbols in gcc
CFLAGS = -g -Iinclude -lws2_32 -DHEXASTRIKE_NULL_CHECKS=1

all: hexastrike.exe 

hexastrike.exe: ${C_SOURCES}
	gcc ${C_SOURCES} -o hexastrike.exe ${CFLAGS}