#pragma once

#include <flecs.h>

void RenderModel(ecs_iter_t* it);

void RenderName(ecs_iter_t* it);

void RenderPainting(ecs_iter_t* it);

void Move(ecs_iter_t* it);

void Animate(ecs_iter_t* it);

void MoveCamera(ecs_iter_t* it);

void KeyboardControls(ecs_iter_t* it);
