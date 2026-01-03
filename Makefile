COMMON_CFLAGS := -Wall -Wextra -Wno-unused-parameter -Wno-override-init -std=gnu11 -I include -I lib/cut/
COMMON_HDR := $(wildcard include/*.h)

CLIENT_CFLAGS := $(COMMON_CFLAGS) -I include/client/ -I lib/flecs/distr/ -I lib/raylib/src/ -I lib/raygui/src/ -lm
CLIENT_HDR := $(COMMON_HDR) $(wildcard include/client/*.h)
CLIENT_SRC := $(wildcard src/client/*.c) lib/flecs/distr/flecs.c lib/raylib/src/libraylib.a

SERVER_CFLAGS := $(COMMON_CFLAGS) -I include/server/ -I lib/cut/ -I lib/raylib/src/
SERVER_HDR := $(COMMON_HDR) $(wildcard include/server/*.h)
SERVER_SRC := $(wildcard src/server/*.c)

OS := $(shell uname -s)

ifeq ($(OS), Darwin)
	CLIENT_CFLAGS += -framework IOKit -framework Cocoa -framework OpenGL
endif

all: client server

client: $(CLIENT_SRC) $(CLIENT_HDR)
	cc $(CLIENT_SRC) $(CLIENT_CFLAGS) -o $@

server: $(SERVER_SRC) $(SERVER_HDR)
	cc $(SERVER_SRC) $(SERVER_CFLAGS) -o $@

client.exe: $(CLIENT_SRC) $(CLIENT_HDR)
	x86_64-w64-mingw32-gcc $(wildcard src/client/*.c) lib/flecs/distr/flecs.c -Wl,--whole-archive lib/raylib/src/libraylib.a -Wl,--no-whole-archive $(COMMON_CFLAGS) -O2 -I include/client/ -I lib/flecs/distr/ -I lib/raylib/src/ -I lib/raygui/src/ -lm -lgdi32 -lwinmm -lws2_32 -limagehlp -static-libgcc -static -o $@
