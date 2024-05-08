/**
 * @file scene.h
 **/

#ifndef _FLUX_SCENE_H_
#define _FLUX_SCENE_H_

// #include "prefabs.h"
#include "transform.h"

typedef enum {
    ONUPDATE,
    AFTERUPDATE,
    ONDESTROY,
    ONDRAW,
    ONDRAW2D
} script_callback_t;

/** @addtogroup sceneapi Scene API
 *  @brief The public API for interacting with game scenes.
 *
 * Before loading a scene, ensure that `flux_reset_scene()` has been called.
 * Then load a scene with `flux_load_scene()`.
 * Close the scene with `flux_close_scene()`, which should be called BEFORE `flux_reset_scene()`.
 *
 *  @{
 */

void flux_load_scene(const char* path);

void flux_reset_scene(void);

void flux_close_scene(void);

/** @} */

void flux_scene_signal_handler(int signal);

void flux_draw_scene(void);

void flux_scene_script_callback(script_callback_t callback);

#endif