#pragma once

#include <message.h>
#include <raylib.h>
#include <stdbool.h>
#include <sock.h>

struct globals {
    char name[ID_BUF_LEN];
    char host[256];
    char port[8];
    socket_t server_sock;
    bool is_connected;
    struct message* message_queue;
    char* recv_buf;
    char* send_buf;
    const Camera* camera;
};

extern struct globals globals;
