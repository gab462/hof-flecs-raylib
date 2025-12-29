#include <config.h>
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

void message_handler(struct task_context *ctx, int fd, struct sockaddr_in addr)
{
    char **bytes = task_ctx_alloc(ctx, char *);

    task_begin(ctx);

    ssize_t received = sock_read(fd, bytes);

    if (received == 0 || (received == -1 && errno != EAGAIN)) { // Connection closed or error
        perror("Lost connection");
        close(fd);
        da_reset(bytes);
        task_abort(ctx);
    }

    while (len(*bytes) > sizeof(struct message)) {
        struct message msg;
        memcpy(&msg, *bytes, sizeof(struct message));

        // TODO: process msg

        // Discard processsed message
        memmove(*bytes, *bytes + sizeof(struct message),
                len(*bytes) - sizeof(struct message));
        da_header(*bytes)->length -= sizeof(struct message);
    }

    task_return(ctx);

    task_end(ctx);
}

int main(void)
{
    int fd = tcp_listen(PORT);
    assert(fd != -1);

    printf("Listening on %s:%d...\n", IP, PORT);

    struct task ctx = {0};
    for (;;) {
        tcp_server(&ctx, PORT, message_handler);
        usleep(8000);
    }

    return 0;
}
