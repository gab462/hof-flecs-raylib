#define RAYGUI_IMPLEMENTATION

#include <components.h>
#include <config.h>
#include <flecs.h>
#include <raylib.h>
#include <raygui.h>
#include <systems.h>
#include <message.h>
#include <globals.h>

struct globals globals = {
    .name = "username",
    .host = "127.0.0.1",
    .port = "8172",
};

void
LoginScreen(void)
{
    int widget_height = 40;
    int widget_padding = 10;

    Rectangle widget_pos = {
        .x = widget_padding,
        .height = widget_height
    };

    bool edit_name = false;
    bool edit_host = false;
    bool edit_port = false;

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    while (!globals.is_connected && !WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        widget_pos.y = widget_padding;

        widget_pos.width = 500;

        if(GuiTextBox(widget_pos, globals.name, sizeof(globals.name), edit_name))
            edit_name = !edit_name;
        widget_pos.y += widget_height + widget_padding;

        if(GuiTextBox(widget_pos, globals.host, sizeof(globals.host), edit_host))
            edit_host = !edit_host;
        widget_pos.y += widget_height + widget_padding;

        if(GuiTextBox(widget_pos, globals.port, sizeof(globals.port), edit_port))
            edit_port = !edit_port;
        widget_pos.y += widget_height + widget_padding;

        widget_pos.width = 200;
        if(GuiButton(widget_pos, "Connect")){
            // TODO: tcp_connect() and handle if connection not successful
            globals.is_connected = true;
        };

        EndDrawing();
    }

    TraceLog(LOG_INFO, "Name: %s, Host: %s, Port: %s", globals.name, globals.host, globals.port);
}

int main(void)
{
    InitWindow(800, 600, "Hall of Fame");

    SetTargetFPS(60);

    LoginScreen();

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

        EndMode3D();
        EndDrawing();
    }

    ecs_fini(ctx);

    return 0;
}
