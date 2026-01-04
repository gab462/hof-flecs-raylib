#define RAYGUI_IMPLEMENTATION

#include <components.h>
#include <config.h>
#include <flecs.h>
#include <globals.h>
#include <login_screen.h>
#include <message.h>
#include <message_processor.h>
#include <peer.h>
#include <raygui.h>
#include <raylib.h>
#include <systems.h>
#include <tcp.h>

int main(void)
{
    InitWindow(800, 600, "Hall of Fame");

    SetTargetFPS(60);

    ecs_world_t* ctx = ecs_init();

    while (!globals.is_connected && !WindowShouldClose()) {
        BeginDrawing();
        LoginScreen(ctx);
        EndDrawing();
    }

    ECS_COMPONENT_DEFINE(ctx, Position);
    ECS_COMPONENT_DEFINE(ctx, Direction);
    ECS_COMPONENT_DEFINE(ctx, WalkingSpeed);
    ECS_COMPONENT_DEFINE(ctx, RotationSpeed);
    ECS_COMPONENT_DEFINE(ctx, Controls);
    ECS_COMPONENT_DEFINE(ctx, AnimationState);
    ECS_COMPONENT_DEFINE(ctx, Model);
    ECS_COMPONENT_DEFINE(ctx, Player);

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

    // TODO: Render player name above model

    ecs_entity_t player = CreatePeer(ctx, globals.name, MODEL_PATH);

    ecs_set(ctx,
        player, Player,
        { .camera = {
              .up = { 0.f, 1.f, 0.f },
              .fovy = 45.f,
              .projection = CAMERA_PERSPECTIVE,
          },
            .distance = 15.f });

    const Player* camera_state = ecs_get(ctx, player, Player);
    const Camera* camera = &camera_state->camera;

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (!globals.is_connected) {
            LoginScreen(ctx);
            EndDrawing();
            continue;
        }

        ClearBackground(RAYWHITE);
        BeginMode3D(*camera);
        DrawGrid(512, 1.f);

        float dt = GetFrameTime();
        ecs_progress(ctx, dt);

        MessageProcessor(ctx);

        EndMode3D();
        EndDrawing();
    }

    ecs_query_t* q = ecs_query(ctx,
        { .terms = {
              { ecs_id(Model) },
              { ecs_id(AnimationState) },
          } });

    ecs_iter_t it = ecs_query_iter(ctx, q);

    while (ecs_query_next(&it)) {
        Model* m = ecs_field(&it, Model, 0);
        AnimationState* s = ecs_field(&it, AnimationState, 1);

        for (int i = 0; i < it.count; i++) {
            UnloadModelAnimations(s[i].animations, s[i].count);
            UnloadModel(m[i]);
        }
    }

    ecs_fini(ctx);

    return 0;
}
