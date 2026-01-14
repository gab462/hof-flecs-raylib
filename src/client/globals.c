#include <globals.h>
#include <sock.h>

struct globals globals = {
    .name = "username",
    .host = "127.0.0.1",
    .port = "8172",
    .server_sock = SOCK_INVALID,
};
