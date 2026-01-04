#include <components.h>
#include <config.h>
#include <errno.h>
#include <flecs.h>
#include <globals.h>
#include <message_processor.h>
#include <sock.h>
#include <sys/socket.h>

void MessageProcessor(ecs_world_t* ctx)
{
    if (globals.server_fd <= 0)
        return;

    ssize_t received = sock_read(globals.server_fd, &globals.recv_buf);

    if (received == 0 || (received == -1 && errno != EAGAIN)) {
        globals.is_connected = false;
        globals.server_fd = -1;
        close(globals.server_fd);
    }

    while (len(globals.recv_buf) >= (int)sizeof(struct message)) {
        struct message msg;
        memcpy(&msg, globals.recv_buf, sizeof(struct message));
        sb_consume(&globals.recv_buf, sizeof(struct message));

        enqueue(&globals.message_queue, msg);
    }

    while (!q_empty(globals.message_queue)) {
        struct message msg = dequeue(&globals.message_queue);

        switch (msg.type) {
        case MESSAGE_HELLO:
            // TODO: instantiate entity
            break;
        case MESSAGE_WELCOME: {
            if (globals.is_connected)
                TraceLog(LOG_WARNING, "Received unexpected message (welcome)");

            struct message_welcome data = msg.data.welcome;
            assert(memcmp(data.to_id, globals.name, sizeof(globals.name)) == 0);

            if (data.accepted) {
                globals.is_connected = true;
            } else {
                shutdown(globals.server_fd, SHUT_WR);
                close(globals.server_fd);
                globals.server_fd = -1;
            }
        } break;
        case MESSAGE_GOODBYE:
            // TODO: remove entity
            break;
        case MESSAGE_GET_STATE:
            // TODO: respond with MESSAGE_SYNC
            break;
        case MESSAGE_SYNC:
            // TODO: update corresponding entity
            break;
        case MESSAGE_TURNING_RIGHT: {
            struct message_turning_right data = msg.data.turning_right;

            ecs_entity_t e = ecs_lookup(ctx, data.from_id);

            if (e == 0) {
                TraceLog(LOG_WARNING, "Entity %s not found", data.from_id);
                break;
            }

            Controls* c = ecs_get_mut(ctx, e, Controls);

            if (data.state == true)
                c->state |= CONTROL_TURNING_RIGHT;
            else
                c->state &= ~CONTROL_TURNING_RIGHT;
        } break;
        case MESSAGE_TURNING_LEFT: {
            struct message_turning_left data = msg.data.turning_left;

            ecs_entity_t e = ecs_lookup(ctx, data.from_id);

            if (e == 0) {
                TraceLog(LOG_WARNING, "Entity %s not found", data.from_id);
                break;
            }

            Controls* c = ecs_get_mut(ctx, e, Controls);

            if (data.state == true)
                c->state |= CONTROL_TURNING_LEFT;
            else
                c->state &= ~CONTROL_TURNING_LEFT;
        } break;
        case MESSAGE_WALKING_FORWARD: {
            struct message_walking_forward data = msg.data.walking_forward;

            ecs_entity_t e = ecs_lookup(ctx, data.from_id);

            if (e == 0) {
                TraceLog(LOG_WARNING, "Entity %s not found", data.from_id);
                break;
            }

            Controls* c = ecs_get_mut(ctx, e, Controls);
            AnimationState* anim = ecs_get_mut(ctx, e, AnimationState);

            if (data.state == true) {
                anim->current = PLAYER_WALKING_ANIMATION;
                c->state |= CONTROL_WALKING_FORWARD;
            } else {
                anim->current = PLAYER_IDLE_ANIMATION;
                c->state &= ~CONTROL_WALKING_FORWARD;
            }
        } break;
        case MESSAGE_WALKING_BACKWARD:
            // TODO: implement backwards animation
            break;
        }
    }

    if (len(globals.send_buf) > 0) {
        sock_write(globals.server_fd, &globals.send_buf);
    }
}
