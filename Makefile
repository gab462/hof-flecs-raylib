COMMON_CFLAGS := -Wall -Wextra -Wshadow -std=gnu11 -I include
COMMON_HDR := include/config.h include/message.h

CLIENT_CFLAGS := $(COMMON_CFLAGS) -I include/client/ -I lib/flecs/distr/ -I lib/raylib/src/
CLIENT_HDR := $(COMMON_HDR) include/client/components.h include/client/systems.h
CLIENT_SRC := src/client/main.c src/client/systems.c lib/flecs/distr/flecs.c lib/raylib/src/libraylib.a -lm

SERVER_CFLAGS := $(COMMON_CFLAGS) -I include/server/
SERVER_SRC := src/server/main.c
SERVER_HDR := $(COMMON_HDR)

OS := $(shell uname -s)

ifeq ($(OS), Darwin)
	CLIENT_CFLAGS += -framework IOKit -framework Cocoa -framework OpenGL
endif

client: $(CLIENT_SRC) $(CLIENT_HDR)
	cc $(CLIENT_CFLAGS) -o $@ $(CLIENT_SRC)
