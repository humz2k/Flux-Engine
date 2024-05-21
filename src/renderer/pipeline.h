/**
 * @file pipeline.h
 **/

#ifndef _FLUX_RENDERER_PIPELINE_H_
#define _FLUX_RENDERER_PIPELINE_H_

#include "raylib.h"
#include "raymath.h"
#include "shader_manager.h"
#include "transform.h"

#define RENDERER_MAX_OBJECTS 10000
#define RENDER_MAX_INSTANCES 10000

/** @addtogroup group1 Renderer API
 *  @brief The public API for interacting with the renderer module.
 *
 * All renderer API functions are prefix with `render`.
 *
 * Usage:
 * * Make sure you call `render_init()` at the start of the program,
 * * and `render_close()` at the end.
 * * To create a `renderModel` from a raylib `Model`, call `render_make_model()`,
 * which returns a `renderModel`.
 * * The basic idea is that we render instances of a `renderModel`.
 * * To reset the number of instances currently for a `renderModel`, call `render_reset_instances()`.
 * * Then, to add an instances, call `render_add_model_instance()`.
 * * When we actually want to render the scene, we first call `render_begin()`, passing the camera to render from.
 * * We then call `render_rmodel()` on any `renderModel`s we want to draw.
 * * After this, we call `render_calculate_shadows()`, which does the shadow calculations.
 * * Finally, we call `render_end()` to draw to the screen.
 *
 *  @{
 */

/** @struct renderModelInternal */
typedef struct renderModelInternal renderModelInternal;

/** @typedef renderModelInternal* renderModel */
typedef renderModelInternal* renderModel;

void render_init(void);

void render_close(void);

void render_begin(Camera3D camera);

void render_rmodel(renderModel rmodel, Color tint);

void render_end(void);

Camera3D render_get_current_cam(void);

void render_draw_grid(int n, float s);

renderModel render_make_model(Model model);

void render_reset_instances(renderModel model);

void render_add_model_instance(renderModel model, fluxTransform transform);

void render_free_model(renderModel model);

int render_get_visible_meshes(void);

/** @} */ // end of group1

void render_draw_all_no_shader(Camera3D camera);

#endif