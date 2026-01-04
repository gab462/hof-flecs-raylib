#pragma once

#include <flecs.h>
#include <raylib.h>

typedef struct {
    float value;
} FloatValue;

enum control_state {
    WALKING_FORWARD = 1,
    WALKING_BACKWARD = 2,
    TURNING_RIGHT = 4,
    TURNING_LEFT = 8,
};

extern ECS_COMPONENT_DECLARE(Model);

typedef Vector3 Position;
extern ECS_COMPONENT_DECLARE(Position);

typedef Vector2 Direction;
extern ECS_COMPONENT_DECLARE(Direction);

typedef FloatValue WalkingSpeed;
extern ECS_COMPONENT_DECLARE(WalkingSpeed);

typedef FloatValue RotationSpeed;
extern ECS_COMPONENT_DECLARE(RotationSpeed);

typedef struct {
    enum control_state state;
} Controls;
extern ECS_COMPONENT_DECLARE(Controls);

typedef struct {
    ModelAnimation* animations;
    int count;
    int current;
    int frame;
} AnimationState;
extern ECS_COMPONENT_DECLARE(AnimationState);

typedef struct {
    Camera camera;
    float distance;
} Player;
extern ECS_COMPONENT_DECLARE(Player);
