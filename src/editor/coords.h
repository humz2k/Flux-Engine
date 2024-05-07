/**
 * @file coords.h
 **/

#ifndef _FLUX_EDITOR_COORDS_H_
#define _FLUX_EDITOR_COORDS_H_

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

typedef enum editorPosType {
    pos_pixel,
    pos_relative,
} editorPosType;

typedef struct editorPos {
    editorPosType t;
    float value;
} editorPos;

editorPos make_pos_pixel(float value);

editorPos make_pos_relative(float value);

typedef struct editorCoord {
    editorPos x, y;
} editorCoord;

editorCoord make_coord(editorPos x, editorPos y);

Vector2 coord_to_vector2(editorCoord coord);

typedef struct editorRect {
    editorCoord p0, p1;
} editorRect;

editorRect make_rect(editorCoord p0, editorCoord p1);

Rectangle editor_rect_to_ray_rect(editorRect rect);

void draw_editor_rect(editorRect rect, Color color, Vector2 offset);

void draw_editor_rect_shadow(editorRect rect, Color color, Vector2 offset,
                             editorPos shadow_size, bool top, bool bottom,
                             bool left, bool right);

bool vector_in_editor_rect(editorRect rect, Vector2 offset, Vector2 coord);

#endif