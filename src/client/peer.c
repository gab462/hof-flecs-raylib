#include <components.h>
#include <config.h>
#include <flecs.h>
#include <peer.h>

ecs_entity_t CreatePeer(ecs_world_t* ctx, char name[ID_BUF_LEN], char* model_path)
{
    ecs_entity_t player = ecs_entity(ctx, { .name = name });

    Model player_model = LoadModel(model_path);
    int player_model_anim_count;
    ModelAnimation* player_model_anims = LoadModelAnimations(model_path, &player_model_anim_count);

    ecs_set(ctx, player, Position, { 0 });
    ecs_set(ctx, player, Position, { 0 });
    ecs_set(ctx, player, Direction, { 1.f, 0.f });
    ecs_set(ctx, player, WalkingSpeed, { 10.f });
    ecs_set(ctx, player, RotationSpeed, { 2.f });
    ecs_set(ctx, player, Controls, { 0 });
    ecs_set(ctx, player, AnimationState,
        {
            .animations = player_model_anims,
            .count = player_model_anim_count,
            .current = PLAYER_IDLE_ANIMATION,
        });
    ecs_set_id(ctx, player, ecs_id(Model), sizeof(Model), &player_model);
    ecs_set(ctx, player, Nametag,
        {
            .name = ecs_get_name(ctx, player),
            .height = 3.f,
        });

    return player;
}

void DestroyPeer(ecs_world_t* ctx, char name[ID_BUF_LEN])
{
    ecs_entity_t peer = ecs_lookup(ctx, name);

    if (peer == 0) {
        TraceLog(LOG_WARNING, "Entity %s not found", name);
        return;
    }

    AnimationState* anim = ecs_get_mut(ctx, peer, AnimationState);
    UnloadModelAnimations(anim->animations, anim->count);

    Model* model = ecs_get_mut(ctx, peer, Model);
    UnloadModel(*model);

    ecs_delete(ctx, peer);
}
