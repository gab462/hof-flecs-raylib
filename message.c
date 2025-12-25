struct message_hello {};

struct message_welcome {
    uint64_t to_id;
};

struct message_get_state {
    uint64_t from_id;
};

struct message_sync {
    uint64_t from_id;
    uint64_t to_id;
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
    uint64_t from_id;
};

struct message_turning_right {
    uint64_t from_id;
    bool state;
};

struct message_turning_left {
    uint64_t from_id;
    bool state;
};

struct message_walking_forward {
    uint64_t from_id;
    bool state;
};

struct message_walking_backward {
    uint64_t from_id;
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
    struct message_hello;
    struct message_welcome;
    struct message_get_state;
    struct message_sync;
    struct message_turning_right;
    struct message_turning_left;
    struct message_walking_forward;
    struct message_walking_backward;
};

struct message {
    enum message_type type;
    union message_data data;
};
