#include <cut.h>
#include <errno.h>
#include <message.h>
#include <sock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <task.h>
#include <tcp_task.h>

void send_message(char **send_buf, struct message msg)
{
    char buf[sizeof(msg)];
    memcpy(buf, &msg, sizeof(msg));
    push_items(send_buf, buf, sizeof(msg));
}

void message_handler(struct task_context *ctx, int fd, struct sockaddr_in addr)
{
    char **recv_buf = task_ctx_alloc(ctx, char *);
    char **send_buf = task_ctx_alloc(ctx, char *);

    task_begin(ctx);

    char *ip = inet_ntoa(addr.sin_addr);

    printf("Received connection from %s\n", ip);

    for(;;){
        ssize_t received = sock_read(fd, recv_buf);

        if (received == 0 || (received == -1 && errno != EAGAIN)) { // Connection closed or error
            perror("Lost connection");
            close(fd);
            da_reset(recv_buf);
            task_abort(ctx);
        }

        while (len(*recv_buf) >= (int) sizeof(struct message)) {
            struct message msg;
            memcpy(&msg, *recv_buf, sizeof(struct message));
            sb_consume(recv_buf, sizeof(struct message));

            switch (msg.type) {
                case MESSAGE_HELLO: {
                    struct message_hello data = msg.data.hello;

                    struct message out = {
                        .type = MESSAGE_WELCOME,
                        .data.welcome.accepted = true // TODO: validate username & IP address
                    };
                    memcpy(out.data.welcome.to_id, data.from_id, sizeof(data.from_id));

                    send_message(send_buf, out);
                }
                break;
                default: assert(false && "TODO");
            }

        }

        if (len(*send_buf) > 0)
            sock_write(fd, send_buf);

        task_yield(ctx);
    }

    task_end(ctx);
}

int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("Usage: %s PORT\n", argv[0]);
        return -1;
    }

    short port = atoi(argv[1]);

    int fd = tcp_listen(port);
    assert(fd != -1);

    printf("Listening on localhost:%d...\n", port);

    struct task_context ctx = {0};
    for (;;) {
        tcp_server(&ctx, fd, message_handler);
        usleep(8000);
    }

    return 0;
}
