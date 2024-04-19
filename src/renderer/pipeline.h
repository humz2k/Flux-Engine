#ifndef _FLUX_RENDERER_PIPELINE_H_
#define _FLUX_RENDERER_PIPELINE_H_

#include "raylib.h"
#include "raymath.h"
#include "shader_manager.h"
#include "transform.h"

#define RENDERER_MAX_OBJECTS 1000
#define RENDER_MAX_INSTANCES 1000

struct renderModelInternal;
typedef struct renderModelInternal* renderModel;

void render_init(void);
void render_close(void);

void render_begin(Camera3D camera);

void render_rmodel(renderModel rmodel, Color tint);

void render_end(void);

void render_draw_all_no_shader(Camera3D camera);

Camera3D render_get_current_cam(void);

void render_draw_grid(int n, float s);

renderModel render_make_model(Model model);

void render_reset_instances(renderModel model);

void render_add_model_instance(renderModel model, fluxTransform transform);

void render_free_model(renderModel model);

int render_get_visible_meshes(void);

#endif