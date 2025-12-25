CFLAGS := -Wall -Wextra -Wshadow -std=gnu11

OS := $(shell uname -s)

ifeq ($(OS), Darwin)
	CFLAGS += -framework IOKit -framework Cocoa -framework OpenGL
endif

client: client.c flecs/distr/flecs.c raylib/src/libraylib.a
	cc $(CFLAGS) -o $@ $^ -I flecs/distr/ -I raylib/src/  # -ggdb -fsanitize=address,undefined
