#pragma once

struct globals {
    char name[16];
    char host[256];
    char port[8];
    int server_fd;
    bool is_connected;
    struct message *message_queue;
    char *recv_buf;
    char *send_buf;
};

extern struct globals globals;
