#pragma once

#include <flecs.h>
#include <raylib.h>

typedef struct {
    float value;
} FloatValue;

enum control_state {
    CONTROL_TURNING_RIGHT = 1,
    CONTROL_TURNING_LEFT = 2,
    CONTROL_WALKING_FORWARD = 4,
    CONTROL_WALKING_BACKWARD = 8,
};

extern ECS_COMPONENT_DECLARE(Model);
extern ECS_COMPONENT_DECLARE(Texture2D);

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

typedef struct {
    const char* name;
    float height;
} Nametag;
extern ECS_COMPONENT_DECLARE(Nametag);

typedef struct {
    Rectangle source;
    Vector2 size;
} TextureParams;
extern ECS_COMPONENT_DECLARE(TextureParams);
