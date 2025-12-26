#include <raylib.h>
#include <flecs.h>
#include <config.h>
#include <components.h>
#include <systems.h>

int
main(void)
{
    InitWindow(800, 600, "Hall of Fame");

    SetTargetFPS(60);

    // TODO: user input for host and port

    // TODO: tcp_connect() with feedback and handle if connection not successful

    ecs_world_t *ctx = ecs_init();

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
    ModelAnimation *player_model_anims = LoadModelAnimations(MODEL_PATH, &player_model_anim_count);

    ecs_entity_t player = ecs_insert(ctx,
                                     ecs_value(Position, {0}),
                                     ecs_value(Direction, { 1.f, 0.f }),
                                     ecs_value(WalkingSpeed, { 10.f }),
                                     ecs_value(RotationSpeed, { 1.f }),
                                     ecs_value(Controls, {0}),
                                     ecs_value(AnimationState, {
                                        .animations = player_model_anims,
                                        .count = player_model_anim_count,
                                        .current = PLAYER_IDLE_ANIMATION
                                     }),
                                     (ecs_value_t){ ecs_id(Model), &player_model });

    ecs_set(ctx, player, Player, {
        .camera = {
            .up = { 0.f, 1.f, 0.f},
            .fovy = 45.f,
            .projection = CAMERA_PERSPECTIVE
        },
        .distance = 15.f
    });

    const Player *camera_state = ecs_get(ctx, player, Player);
    const Camera *camera = &camera_state->camera;

    while(!WindowShouldClose()){
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

    return(0);
}
