#pragma once

typedef struct {
    float value;
} FloatValue;

enum control_state {
    WALKING_FORWARD = 1,
    WALKING_BACKWARD = 2,
    TURNING_RIGHT = 4,
    TURNING_LEFT = 8,
};

typedef Vector3 Position;
typedef Vector2 Direction;

typedef FloatValue WalkingSpeed;
typedef FloatValue RotationSpeed;

typedef struct {
    enum control_state state;
} Controls;

typedef struct {
    ModelAnimation *animations;
    int count;
    int current;
    int frame;
} AnimationState;

typedef struct {
    Camera camera;
    float distance;
} Player;
