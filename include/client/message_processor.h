#pragma once

#include <cut.h>
#include <flecs.h>
#include <message.h>

#define enqueue_message(q, msg, ...) enqueue(q, make_message(msg, __VA_ARGS__))

void MessageProcessor(ecs_world_t* ctx);
