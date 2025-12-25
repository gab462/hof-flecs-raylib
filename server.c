bool
message_handler(int fd, struct sockaddr_in addr, void **data)
{
    struct handler *self = (struct tcp_cli_conn *) interface;
    char **bytes = (char **) data;

    ssize_t received = sock_read(self->fd, bytes);

    if(received == 0 || (received == -1 && errno != EAGAIN)){ // Connection closed or error
        perror("Lost connection");
        close(self->fd);
        da_reset(bytes);
        return(true);
    }

    while(len(*bytes) > sizeof(struct message)){
        struct message msg;
        memcpy(&msg, *bytes, sizeof(struct message));

        // TODO: process msg

        // Discard processsed message
        memmove(*bytes, *bytes + sizeof(struct message), da_len(*bytes) - sizeof(struct message));
        da_header(*bytes)->length -= sizeof(struct message);
    }

    return(false);
}

int
main(void)
{
    short port = atoi(PORT);

    struct task *task = tcp_server(PORT, message_handler);

    printf("Listening on %s:%s...\n", IP, PORT);

    while(!task_poll(task))
        usleep(8000);

    free(task);

    return(0);
}
