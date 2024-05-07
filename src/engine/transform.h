/**
 * @file transform.h
 **/

#ifndef _FLUX_TRANSFORM_H_
#define _FLUX_TRANSFORM_H_

#include "raylib.h"
#include "raymath.h"

typedef struct fluxTransform {
    Vector3 pos;
    Vector3 rot;
    Vector3 scale;
} fluxTransform;

static inline fluxTransform flux_empty_transform(void) {
    fluxTransform out;
    out.pos = Vector3Zero();
    out.rot = Vector3Zero();
    out.scale = Vector3One();
    return out;
}

#endif