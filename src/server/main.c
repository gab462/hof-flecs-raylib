#include <arpa/inet.h>
#include <assert.h>
#include <cut.h>
#include <errno.h>
#include <message.h>
#include <sock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <task.h>
#include <tcp_task.h>

struct peer {
    char name[ID_BUF_LEN];
    int fd;
    char* recv_buf;
    char* send_buf;
};

struct peer** peers = NULL;

void disconnect_peer(struct peer* peer)
{
    close(peer->fd);
    da_reset(&peer->recv_buf);
    da_reset(&peer->send_buf);

    for (int i = 0; i < len(peers); i++) {
        if (peers[i] == peer)
            swap_delete(&peers, i);
    }

    foreach (player, peers) {
        send_message(&(*player)->send_buf,
            ((struct message) {
                .type = MESSAGE_GOODBYE,
            }),
            .from_id = peer->name);
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

void message_handler(struct task_context* ctx, int fd, struct sockaddr_in addr)
{
    struct peer* self = task_ctx_alloc(ctx, struct peer);

    task_begin(ctx);

    char* ip = inet_ntoa(addr.sin_addr);

    printf("Received connection from %s\n", ip);

    self->fd = fd;

    push(&peers, self);

    for (;;) {
        ssize_t received = sock_read(fd, &self->recv_buf);

        if (received == 0 || (received == -1 && errno != EAGAIN)) { // Connection closed or error
            printf("Lost connection to peer\n");
            disconnect_peer(self);
            task_abort(ctx);
        }

        while (len(self->recv_buf) >= (int)sizeof(struct message)) {
            struct message msg;
            memcpy(&msg, self->recv_buf, sizeof(struct message));
            sb_consume(&self->recv_buf, sizeof(struct message));

            if (msg.type < MESSAGE_HELLO || msg.type > MESSAGE_WALKING_BACKWARD) {
                fprintf(stderr, "Received message with invalid type from %s\n", self->name);
                disconnect_peer(self);
                task_abort(ctx);
            }

            switch (msg.type) {
            case MESSAGE_HELLO: {
                struct message_hello data = msg.data.hello;

                send_message(&self->send_buf,
                    ((struct message) {
                        .type = MESSAGE_WELCOME,
                        .data.welcome.accepted = true,
                    }),
                    .to_id = data.from_id);

                printf("Player '%s' joined\n", data.from_id);

                snprintf(self->name, ID_BUF_LEN, "%s", data.from_id);

                // TODO: forward hello message to all peers
                // TODO: send hello from all peers to new player
                // TODO: send get_state message to all peers
            } break;
            case MESSAGE_WELCOME:
                printf("Client sent unexpected message (welcome)\n");
                disconnect_peer(self);
                task_abort(ctx);
                // TODO: broadcast goodbye
                break;
            case MESSAGE_GOODBYE:
                printf("Client sent unexpected message (goodbye)\n");
                disconnect_peer(self);
                task_abort(ctx);
                break;
            case MESSAGE_GET_STATE:
                // TODO: broadcast to all except sender
                break;
            case MESSAGE_SYNC:
                // TODO: send response to to_id
                break;
            case MESSAGE_TURNING_RIGHT:
                // TODO: broadcast to all except sender
                break;
            case MESSAGE_TURNING_LEFT:
                // TODO: broadcast to all except sender
                break;
            case MESSAGE_WALKING_FORWARD:
                // TODO: broadcast to all except sender
                break;
            case MESSAGE_WALKING_BACKWARD:
                // TODO: broadcast to all except sender
                break;
            }
        }

        if (len(self->send_buf) > 0)
            sock_write(fd, &self->send_buf);

        task_yield(ctx);
    }

    task_end(ctx);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Usage: %s PORT\n", argv[0]);
        return -1;
    }

    short port = atoi(argv[1]);

    int fd = tcp_listen(port);
    assert(fd != -1);

    printf("Listening on localhost:%d...\n", port);

    struct task_context ctx = { 0 };
    for (;;) {
        tcp_server(&ctx, fd, message_handler);
        usleep(8000);
    }

    return 0;
}
