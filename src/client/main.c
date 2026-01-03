#define RAYGUI_IMPLEMENTATION

#include <errno.h>
#include <components.h>
#include <config.h>
#include <systems.h>
#include <message.h>
#include <globals.h>
#include <flecs.h>
#include <raylib.h>
#include <raygui.h>
#include <tcp.h>

struct globals globals = {
    .name = "username",
    .host = "127.0.0.1",
    .port = "8172",
};

void SendMessage(struct message msg)
{
    char buf[sizeof(msg)];
    memcpy(buf, &msg, sizeof(msg));
    push_items(&globals.send_buf, buf, sizeof(msg));
}

void MessageProcessor(void)
{
    if(globals.server_fd <= 0)
        return;

    ssize_t received = sock_read(globals.server_fd, &globals.recv_buf);

    if (received == 0 || (received == -1 && errno != EAGAIN)) {
            globals.is_connected = false;
            globals.server_fd = -1;
            close(globals.server_fd);
    }

    while (len(globals.recv_buf) >= (int) sizeof(struct message)) {
        struct message msg;
        memcpy(&msg, globals.recv_buf, sizeof(struct message));
        sb_consume(&globals.recv_buf, sizeof(struct message));

        enqueue(&globals.message_queue, msg);
    }

    while (!q_empty(globals.message_queue)) {
        struct message msg = dequeue(&globals.message_queue);

        switch (msg.type) {
            case MESSAGE_WELCOME:
                assert(!globals.is_connected);

                struct message_welcome data = msg.data.welcome;
                assert(memcmp(data.to_id, globals.name, sizeof(globals.name)) == 0);

                if (data.accepted) {
                    globals.is_connected = true;
                } else {
                    shutdown(globals.server_fd, SHUT_WR);
                    close(globals.server_fd);
                    globals.server_fd = -1;
                }

                break;
            default: assert(false && "TODO");
        }
    }

    while (len(globals.send_buf) > 0) {
        sock_write(globals.server_fd, &globals.send_buf);
    }
}

void LoginScreen(void)
{
    static bool edit_name = false;
    static bool edit_host = false;
    static bool edit_port = false;

    int widget_padding = 10;

    Rectangle widget_pos = {
        .x = widget_padding,
        .y = widget_padding,
        .height = 40
    };

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    widget_pos.width = 500;

    if (GuiTextBox(widget_pos, globals.name, sizeof(globals.name), edit_name))
        edit_name = !edit_name;
    widget_pos.y += widget_pos.height + widget_padding;

    if (GuiTextBox(widget_pos, globals.host, sizeof(globals.host), edit_host))
        edit_host = !edit_host;
    widget_pos.y += widget_pos.height + widget_padding;

    if (GuiTextBox(widget_pos, globals.port, sizeof(globals.port), edit_port))
        edit_port = !edit_port;
    widget_pos.y += widget_pos.height + widget_padding;

    widget_pos.width = 200;

    if (GuiButton(widget_pos, "Connect") && globals.server_fd <= 0) {
        globals.server_fd = tcp_connect(globals.host, globals.port);

        if (globals.server_fd > 0) {
            TraceLog(LOG_INFO, "Sending Hello message");
            struct message out = {
                .type = MESSAGE_HELLO,
                .data.hello.from_id = {0}
            };

            memcpy(out.data.hello.from_id, globals.name, sizeof(globals.name));

            SendMessage(out);
        }
    }
    widget_pos.y += widget_pos.height + widget_padding;

    if (globals.server_fd == -1)
        GuiLabel(widget_pos, "Failed to connect");
    else if (globals.server_fd > 0)
        GuiLabel(widget_pos, "Connecting...");

    widget_pos.y += widget_pos.height + widget_padding;

    if(globals.server_fd > 0){
        MessageProcessor();
    }
}

int main(void)
{
    InitWindow(800, 600, "Hall of Fame");

    SetTargetFPS(60);

    while (!globals.is_connected && !WindowShouldClose()) {
        BeginDrawing();
        LoginScreen();
        EndDrawing();
    }

    ecs_world_t* ctx = ecs_init();

    ECS_COMPONENT(ctx, Position);
    ECS_COMPONENT(ctx, Direction);
    ECS_COMPONENT(ctx, WalkingSpeed);
    ECS_COMPONENT(ctx, RotationSpeed);
    ECS_COMPONENT(ctx, Controls);
    ECS_COMPONENT(ctx, AnimationState);
    ECS_COMPONENT(ctx, Model);
    ECS_COMPONENT(ctx, Player);

    // TODO: billboard component

    ECS_SYSTEM(ctx, KeyboardControls, EcsOnUpdate,
        Controls, [in] Player);
    ECS_SYSTEM(ctx, Move, EcsOnUpdate,
        Position, Direction, [in] WalkingSpeed, [in] RotationSpeed, [in] Controls);
    ECS_SYSTEM(ctx, Animate, EcsOnUpdate,
        Model, AnimationState);
    ECS_SYSTEM(ctx, RenderModel, EcsOnUpdate,
        [in] Position, [in] Direction, [in] Model);
    ECS_SYSTEM(ctx, MoveCamera, EcsOnUpdate,
        [in] Position, [in] Direction, Player);

    // TODO: connection task
    // TODO: message processor system - creation, removal, control state, animations

    Model player_model = LoadModel(MODEL_PATH);
    int player_model_anim_count;
    ModelAnimation* player_model_anims = LoadModelAnimations(MODEL_PATH, &player_model_anim_count);

    ecs_entity_t player = ecs_insert(ctx,
        ecs_value(Position, { 0 }),
        ecs_value(Direction, { 1.f, 0.f }),
        ecs_value(WalkingSpeed, { 10.f }),
        ecs_value(RotationSpeed, { 1.f }),
        ecs_value(Controls, { 0 }),
        ecs_value(AnimationState, {
            .animations = player_model_anims,
            .count = player_model_anim_count,
            .current = PLAYER_IDLE_ANIMATION
        }),
        (ecs_value_t) { ecs_id(Model), &player_model });

    ecs_set(ctx, player, Player, { .camera = { .up = { 0.f, 1.f, 0.f }, .fovy = 45.f, .projection = CAMERA_PERSPECTIVE }, .distance = 15.f });

    const Player* camera_state = ecs_get(ctx, player, Player);
    const Camera* camera = &camera_state->camera;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(*camera);
        DrawGrid(512, 1.f);

        float dt = GetFrameTime();
        ecs_progress(ctx, dt);

        // TODO: process messages

        EndMode3D();
        EndDrawing();
    }

    ecs_fini(ctx);

    return 0;
}
