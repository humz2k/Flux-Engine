#ifndef _FLUX_RENDERER_PIPELINE_H_
#define _FLUX_RENDERER_PIPELINE_H_

#include "raylib.h"
#include "raymath.h"
#include "transform.h"
#include "shader_manager.h"

#define RENDERER_MAX_OBJECTS 1000

void render_init(void);
void render_close(void);

void render_begin(Camera3D camera);

void render_model(Model model, fluxTransform transform, Color tint);

void render_end(void);

#endif