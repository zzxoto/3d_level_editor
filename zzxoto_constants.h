#ifndef ZZXOTO_CONSTANTS_H
#define ZZXOTO_CONSTANTS_H

#include "zzxoto_vector.h"

const Vec3 CAMERA_INITIAL_POSITION = {-20.62f, 62.68f, -18.69f};
const Vec3 CAMERA_INITIAL_LOOKAT =   {.21f, -.71f, .67f};
const float CAMERA_MOVE_VELOCITY = 60.f;
const float CAMERA_ROTATE_VELOCITY = 60.f;
const float NEAR = .01f;
const float FAR = 1000.f;

const float FLOOR_WIDTH = 100.f;
const float FLOOR_LENGTH = 100.f;
const Vec3 FLOOR_CENTER  = { 0.f, 0.f, 0.f };
const Vec3 FLOOR_NORMAL  = { 0.f, 1.f, 0.f };

const Vec3 COLOR_FLOOR_INITIAL = {.2824f, .4157f, .2784f};
const Vec3 COLOR_BACKGROUND_INITIAL = { .8f, .8f, .8f };
const Vec3 COLOR_HIGHLIGHT_MESH = {163.f/255.f, 142.f/255.f, 170.f/255.f};
const Vec3 COLOR_COPYING_BOX = {1.0f, 1.0f, 0.f};
#endif