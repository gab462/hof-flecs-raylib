#pragma once

#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>

struct message_hello {
    char from_id[16];
};

struct message_welcome {
    char to_id[16];
    bool accepted;
};

struct message_get_state {
    char from_id[16];
};

struct message_sync {
    char from_id[16];
    char to_id[16];
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
    char from_id[16];
};

struct message_turning_right {
    char from_id[16];
    bool state;
};

struct message_turning_left {
    char from_id[16];
    bool state;
};

struct message_walking_forward {
    char from_id[16];
    bool state;
};

struct message_walking_backward {
    char from_id[16];
    bool state;
};

enum message_type {
    MESSAGE_HELLO,
    MESSAGE_WELCOME,
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
