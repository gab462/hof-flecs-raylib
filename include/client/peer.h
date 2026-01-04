#pragma once

#include <flecs.h>
#include <message.h>

ecs_entity_t CreatePeer(ecs_world_t* ctx, char name[ID_BUF_LEN], char* model_path);

void DestroyPeer(ecs_world_t* ctx, char name[ID_BUF_LEN]);
