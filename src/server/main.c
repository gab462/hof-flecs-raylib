#include <assert.h>
#include <cut.h>
#include <message.h>
#include <server_peer.h>
#include <sock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcp_task.h>

void message_handler(struct task_context* ctx, socket_t sock, struct sockaddr_in addr)
{
    struct peer* self = task_ctx_alloc(ctx, struct peer);

    task_begin(ctx);

    char* ip = inet_ntoa(addr.sin_addr);
    // TODO: optional whitelist

    printf("Received connection from %s\n", ip);

    self->sock = sock;

    for (;;) {
        int received = sock_read(sock, &self->recv_buf);

        if (received == 0 || (received == -1 && sock_error() != SOCK_WOULDBLOCK)) { // Connection closed or error
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

                bool accepted = true;

                if (!unique_peer_name(data.from_id)) {
                    printf("Player tried to connect with duplicate name (%s)\n", data.from_id);
                    accepted = false; // Do not accept if duplicate name
                }

                send_message(&self->send_buf,
                    ((struct message) {
                        .type = MESSAGE_WELCOME,
                        .data.welcome.accepted = accepted,
                    }),
                    .to_id = data.from_id);

                // Close connection and exit if not accepted
                if (!accepted) {
                    disconnect_peer(self);
                    task_abort(ctx);
                    break;
                }

                snprintf(self->name, ID_BUF_LEN, "%s", data.from_id);

                connect_peer(self, msg);

                printf("Player '%s' joined\n", data.from_id);
            } break;
            case MESSAGE_WELCOME:
                printf("Client sent unexpected message (welcome)\n");
                disconnect_peer(self);
                task_abort(ctx);
                break;
            case MESSAGE_GOODBYE:
                printf("Client sent unexpected message (goodbye)\n");
                disconnect_peer(self);
                task_abort(ctx);
                break;
            case MESSAGE_SYNC: {
                struct peer* peer = get_peer(msg.data.sync.to_id);
                if (peer == NULL) {
                    printf("Peer %s not found\n", msg.data.sync.to_id);
                    break;
                }

                send_message(&peer->send_buf, msg);
            } break;
            default:
                broadcast_message(msg, self);
                break;
            }
        }

        if (len(self->send_buf) > 0)
            sock_write(sock, &self->send_buf);

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

    sock_init();

    short port = atoi(argv[1]);

    socket_t sock = tcp_listen(port);
    assert(sock != SOCK_INVALID);

    printf("Listening on localhost:%d...\n", port);

    struct task_context ctx = { 0 };
    for (;;) {
        tcp_server(&ctx, sock, message_handler);
        usleep(8000);
    }

    return 0;
}
