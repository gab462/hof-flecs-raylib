#include "message.h"
#include <cut.h>
#include <server_peer.h>
#include <sock.h>
#include <stdio.h>

struct peer** peers = NULL;

void broadcast_message(struct message msg, struct peer* sender)
{
    foreach (peer, peers) {
        if (*peer == sender)
            continue;

        send_message(&(*peer)->send_buf, msg);
    }
}

void connect_peer(struct peer* self, struct message hello_message)
{
    foreach (peer, peers) {
        // Instantiate user in peer session
        send_message(&(*peer)->send_buf, hello_message);

        // Instantiate peer in user session
        send_message(&self->send_buf,
            ((struct message) {
                .type = MESSAGE_HELLO,
            }),
            .from_id = (*peer)->name);

        // Sync state from peer in user session
        send_message(&(*peer)->send_buf,
            ((struct message) {
                .type = MESSAGE_GET_STATE,
            }),
            .from_id = self->name);
    }

    push(&peers, self);
}

void disconnect_peer(struct peer* peer)
{
    sock_close(peer->sock);
    da_reset(&peer->recv_buf);
    da_reset(&peer->send_buf);

    bool in_world = false;

    for (int i = 0; i < len(peers); i++) {
        if (peers[i] == peer) {
            swap_delete(&peers, i);
            in_world = true;
        }
    }

    if (in_world) {
        foreach (player, peers) {
            send_message(&(*player)->send_buf,
                ((struct message) {
                    .type = MESSAGE_GOODBYE,
                }),
                .from_id = peer->name);
        }
    }
}

struct peer* get_peer(char name[ID_BUF_LEN])
{
    foreach (peer, peers) {
        if (strncmp(name, (*peer)->name, ID_BUF_LEN) == 0)
            return *peer;
    }

    printf("Peer %s not found\n", name);

    return NULL;
}

bool unique_peer_name(char name[ID_BUF_LEN])
{
    foreach (peer, peers) {
        if (strncmp((*peer)->name, name, ID_BUF_LEN) == 0) {
            return false;
        }
    }

    return true;
}
