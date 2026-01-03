#pragma once

#include <message.h>
#include <stdbool.h>

struct globals {
    char name[ID_BUF_LEN];
    char host[256];
    char port[8];
    int server_fd;
    bool is_connected;
    struct message* message_queue;
    char* recv_buf;
    char* send_buf;
};

extern struct globals globals;
