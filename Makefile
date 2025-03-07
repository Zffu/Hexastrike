# Platform detection
ifeq ($(OS),Windows_NT)
    PLATFORM = Windows
    LIB_EXT = .lib
    EXE_EXT = .exe
    RM = del /Q
    LIB_CMD = ar rcs $@
    PLATFORM_LIBS = -lws2_32
else
    PLATFORM = Unix
    LIB_EXT = .a
    EXE_EXT =
    RM = rm -f
    LIB_CMD = ar rcs $@
    PLATFORM_LIBS = 
endif

# Project settings
LIB_NAME = hexastrike$(LIB_EXT)
TARGET = hexastrike$(EXE_EXT)

# Source files
C_SOURCES = $(wildcard src/*.c src/**/*.c src/**/**/*.c example/*.c)
HEADERS = $(wildcard include/*.h include/*/*.h include/*/*/*.h)
LIB_SOURCES = $(filter-out example/%.c, $(C_SOURCES))
OBJ = ${C_SOURCES:.c=.o}
LIB_OBJECTS = ${LIB_SOURCES:.c=.o}

# Compiler flags
HEXASTRIKE_DEBUG_FLAGS = -DHEXASTRIKE_NULL_CHECKS=1 -DHEXASTRIKE_DEBUG_LOGS=1
HEXASTRIKE_FLAGS = -DHEXASTRIKE_IO_THREAD_POOL_MEMBERS=2 -DHEXASTRIKE_IO_BUFFER_SIZE=2048 -DHEXASTIRKE_IOTHREAD_INDICATOR="unsigned char" -DHEXASTRIKE_SOFT_IO_THREAD=1
COMMON_FLAGS = -Iinclude $(HEXASTRIKE_FLAGS)
DEBUG_FLAGS = $(COMMON_FLAGS) -g -O0 $(HEXASTRIKE_DEBUG_FLAGS)

HEXASTIRKE_TEST_FLAGS = -DHEXASTRIKE_NO_D_HANDLER=1 -DHEXASTRIKE_NO_R_HANDLER=1

# Targets
.PHONY: all lib clean

all: $(TARGET)

lib: $(LIB_NAME)

%.o: %.c $(HEADERS)
	$(CC) $(DEBUG_FLAGS) -c $< -o $@

$(LIB_NAME): $(LIB_OBJECTS)
	$(LIB_CMD) $(LIB_OBJECTS)

$(TARGET): $(C_SOURCES)
	gcc $(C_SOURCES) -o $(TARGET) $(DEBUG_FLAGS) $(HEXASTIRKE_TEST_FLAGS) $(PLATFORM_LIBS)

clean:
	$(RM) $(subst /,\,$(OBJ)) $(TARGET) $(LIB_NAME)