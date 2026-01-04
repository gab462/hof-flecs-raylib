#pragma once

#include "cut.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ID_BUF_LEN 16

struct message_hello {
    char from_id[ID_BUF_LEN];
};

struct message_welcome {
    char to_id[ID_BUF_LEN];
    bool accepted;
};

struct message_goodbye {
    char from_id[ID_BUF_LEN];
};

struct message_get_state {
    char from_id[ID_BUF_LEN];
};

struct message_sync {
    char from_id[ID_BUF_LEN];
    char to_id[ID_BUF_LEN];
    Vector3 position;
    Vector2 direction;
    float speed;
    float rotation_speed;
    bool is_turning_right;
    bool is_turning_left;
    bool is_walking_forward;
    bool is_walking_backward;
};

struct message_left {
    char from_id[ID_BUF_LEN];
};

struct message_turning_right {
    char from_id[ID_BUF_LEN];
    bool state;
};

struct message_turning_left {
    char from_id[ID_BUF_LEN];
    bool state;
};

struct message_walking_forward {
    char from_id[ID_BUF_LEN];
    bool state;
};

struct message_walking_backward {
    char from_id[ID_BUF_LEN];
    bool state;
};

enum message_type {
    MESSAGE_HELLO,
    MESSAGE_WELCOME,
    MESSAGE_GOODBYE,
    MESSAGE_GET_STATE,
    MESSAGE_SYNC,
    MESSAGE_TURNING_RIGHT,
    MESSAGE_TURNING_LEFT,
    MESSAGE_WALKING_FORWARD,
    MESSAGE_WALKING_BACKWARD
};

union message_data {
    struct message_hello hello;
    struct message_welcome welcome;
    struct message_goodbye goodbye;
    struct message_get_state get_state;
    struct message_sync sync;
    struct message_turning_right turning_right;
    struct message_turning_left turning_left;
    struct message_walking_forward walking_forward;
    struct message_walking_backward walking_backward;
};

struct message {
    enum message_type type;
    union message_data data;
};

struct message_opt {
    char* from_id;
    char* to_id;
};

#define make_message(msg, ...) make_message_impl(msg, (struct message_opt) { 0 __VA_OPT__(, ) __VA_ARGS__ })

static inline struct message make_message_impl(struct message msg, struct message_opt opt)
{
    if (opt.from_id != NULL || opt.to_id != NULL) {
        switch (msg.type) {
        case MESSAGE_HELLO:
            snprintf(msg.data.hello.from_id, ID_BUF_LEN, "%s", opt.from_id);
            break;
        case MESSAGE_WELCOME:
            snprintf(msg.data.welcome.to_id, ID_BUF_LEN, "%s", opt.to_id);
            break;
        case MESSAGE_GOODBYE:
            snprintf(msg.data.goodbye.from_id, ID_BUF_LEN, "%s", opt.from_id);
            break;
        case MESSAGE_GET_STATE:
            snprintf(msg.data.get_state.from_id, ID_BUF_LEN, "%s", opt.from_id);
            break;
        case MESSAGE_SYNC:
            snprintf(msg.data.sync.from_id, ID_BUF_LEN, "%s", opt.from_id);
            snprintf(msg.data.sync.to_id, ID_BUF_LEN, "%s", opt.to_id);
            break;
        case MESSAGE_TURNING_RIGHT:
            snprintf(msg.data.turning_right.from_id, ID_BUF_LEN, "%s", opt.from_id);
            break;
        case MESSAGE_TURNING_LEFT:
            snprintf(msg.data.turning_left.from_id, ID_BUF_LEN, "%s", opt.from_id);
            break;
        case MESSAGE_WALKING_FORWARD:
            snprintf(msg.data.walking_forward.from_id, ID_BUF_LEN, "%s", opt.from_id);
            break;
        case MESSAGE_WALKING_BACKWARD:
            snprintf(msg.data.walking_backward.from_id, ID_BUF_LEN, "%s", opt.from_id);
            break;
        }
    }

    return msg;
}

#define send_message(buf, msg, ...) send_message_impl(buf, make_message(msg, __VA_ARGS__))

static inline void send_message_impl(char** send_buf, struct message msg)
{
    char buf[sizeof(msg)] = { 0 };
    int count = sizeof(msg);

    memcpy(buf, &msg, count);
    push_items(send_buf, buf, count);
}
