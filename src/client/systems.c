#include <components.h>
#include <cut.h>
#include <flecs.h>
#include <globals.h>
#include <math.h>
#include <message.h>
#include <raylib.h>
#include <raymath.h>
#include <systems.h>

void RenderModel(ecs_iter_t* it)
{
    Position* p = ecs_field(it, Position, 0);
    Direction* d = ecs_field(it, Direction, 1);
    Model* m = ecs_field(it, Model, 2);

    for (int i = 0; i < it->count; i++) {
        DrawModelEx(m[i], p[i], (Vector3) { 0.f, 1.f, 0.f },
            atan2f(d[i].x, d[i].y) * (180.f / M_PI),
            (Vector3) { .5f, .5f, .5f }, WHITE);
    }
}

void Move(ecs_iter_t* it)
{
    Position* p = ecs_field(it, Position, 0);
    Direction* d = ecs_field(it, Direction, 1);
    WalkingSpeed* ws = ecs_field(it, WalkingSpeed, 2);
    RotationSpeed* rs = ecs_field(it, RotationSpeed, 3);
    Controls* c = ecs_field(it, Controls, 4);

    for (int i = 0; i < it->count; i++) {
        if (c[i].state & WALKING_FORWARD) {
            p[i] = Vector3Add(p[i],
                (Vector3) {
                    ws[i].value * it->delta_time * d[i].x,
                    0.f,
                    ws[i].value * it->delta_time * d[i].y,
                });
        }

        if (c[i].state & WALKING_BACKWARD) {
            p[i] = Vector3Add(p[i],
                (Vector3) {
                    -ws[i].value * it->delta_time * d[i].x,
                    0.f,
                    -ws[i].value * it->delta_time * d[i].y,
                });
        }

        if (c[i].state & TURNING_RIGHT) {
            d[i] = Vector2Rotate(d[i], rs[i].value * it->delta_time);
        }

        if (c[i].state & TURNING_LEFT) {
            d[i] = Vector2Rotate(d[i], -rs[i].value * it->delta_time);
        }
    }
}

void Animate(ecs_iter_t* it)
{
    Model* m = ecs_field(it, Model, 0);
    AnimationState* s = ecs_field(it, AnimationState, 1);

    for (int i = 0; i < it->count; i++) {
        s[i].frame = (s[i].frame + 1) % s[i].animations[s[i].current].frameCount;
        UpdateModelAnimation(m[i], s[i].animations[s[i].current], s[i].frame);
    }
}

void MoveCamera(ecs_iter_t* it)
{
    Position* p = ecs_field(it, Position, 0);
    Direction* d = ecs_field(it, Direction, 1);
    Player* pl = ecs_field(it, Player, 2);

    for (int i = 0; i < it->count; i++) {
        pl[i].camera.position = (Vector3) {
            p[i].x - d[i].x * pl[i].distance,
            pl[i].distance / 2.f,
            p[i].z - d[i].y * pl[i].distance,
        };

        pl[i].camera.target = p[i];
    }
}

void KeyboardControls(ecs_iter_t* it)
{
    Controls* c = ecs_field(it, Controls, 0);
    // Player *p = ecs_field(it, Player, 1);

    for (int i = 0; i < it->count; i++) {
        // TODO: send message instead of changing state in place
        // (sync with animation state)
        // if (IsKeyPressed(KEY_W)) {
        //     enqueue_message(&globals.message_queue,
        //         ((struct message) {
        //             .type = MESSAGE_WALKING_FORWARD,
        //             .data.walking_forward.state = true,
        //         }), .from_id = globals.name);
        //
        //     send_message(&globals.send_buf,
        //         ((struct message) {
        //             .type = MESSAGE_WALKING_FORWARD,
        //             .data.walking_forward.state = true,
        //         }), .from_id = globals.name);
        // }

        c->state
            = IsKeyDown(KEY_W) | (IsKeyDown(KEY_S) << 1) | (IsKeyDown(KEY_D) << 2) | (IsKeyDown(KEY_A) << 3);
    }
}
