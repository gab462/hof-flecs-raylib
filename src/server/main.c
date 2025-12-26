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

bool message_handler(int fd, struct sockaddr_in addr, void** data)
{
    char** bytes = (char**)data;

    ssize_t received = sock_read(fd, bytes);

    if (received == 0 || (received == -1 && errno != EAGAIN)) { // Connection closed or error
        perror("Lost connection");
        close(fd);
        da_reset(bytes);
        return (true);
    }

    while (len(*bytes) > sizeof(struct message)) {
        struct message msg;
        memcpy(&msg, *bytes, sizeof(struct message));

        // TODO: process msg

        // Discard processsed message
        memmove(*bytes, *bytes + sizeof(struct message), da_len(*bytes) - sizeof(struct message));
        da_header(*bytes)->length -= sizeof(struct message);
    }

    return false;
}

int main(void)
{
    struct task* task = tcp_server(PORT, message_handler);

    printf("Listening on %s:%d...\n", IP, PORT);

    while (!task_poll(task))
        usleep(8000);

    free(task);

    return 0;
}
