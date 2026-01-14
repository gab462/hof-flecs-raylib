#pragma once

#include <cut.h>
#include <message.h>
#include <sock.h>

struct peer {
    char name[ID_BUF_LEN];
    socket_t sock;
    char* recv_buf;
    char* send_buf;
};

void broadcast_message(struct message msg, struct peer* sender);
void connect_peer(struct peer* peer, struct message hello_message);
void disconnect_peer(struct peer* peer);
struct peer* get_peer(char name[ID_BUF_LEN]);
bool unique_peer_name(char name[ID_BUF_LEN]);
