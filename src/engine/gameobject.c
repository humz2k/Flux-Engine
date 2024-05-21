/*
                                   _     _           _
                                  | |   (_)         | |
   __ _  __ _ _ __ ___   ___  ___ | |__  _  ___  ___| |_
  / _` |/ _` | '_ ` _ \ / _ \/ _ \| '_ \| |/ _ \/ __| __|
 | (_| | (_| | | | | | |  __/ (_) | |_) | |  __/ (__| |_
  \__, |\__,_|_| |_| |_|\___|\___/|_.__/| |\___|\___|\__|
   __/ |                               _/ |
  |___/                               |__/
*/

/**
 * @file gameobject.c
 * @brief Manages game objects including their properties, transformations,
 * models, cameras, scripts, and children.
 *
 * Provides functions to allocate, initialize, and manipulate game objects
 * within a scene. Supports setting models and cameras, adding scripts and child
 * objects, and accessing various properties of game objects.
 */

#include "gameobject.h"
#include "config.h"
#include "hqtools/hqtools.h"
#include "pipeline.h"
#include "prefabs.h"
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "sceneallocator.h"
#include "transform.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @struct fluxGameObjectStruct
 * @brief Represents a game object within a scene.
 *
 * Game objects can have models, cameras, scripts, and child objects, enabling
 * them to represent a wide variety of elements within the game.
 */
struct fluxGameObjectStruct {
    int id; ///< Unique ID of the game object.
    fluxTransform
        transform; ///< Transformation data for position, rotation, and scaling.
    renderModel model;   ///< Renderable model associated with the game object.
    int n_scripts;       ///< Number of scripts attached to the game object.
    fluxScript* scripts; ///< Array of scripts attached to the game object.
    bool is_camera; ///< Flag indicating whether the game object functions as a
                    ///< camera.
    float fov;      ///< Field of view, relevant if the object is a camera.
    int projection; ///< Camera projection type (e.g., orthographic or
                    ///< perspective).
    bool visible;
};

/**
 * @brief Retrieves the unique ID of a game object.
 * @param obj Pointer to the game object.
 * @return Unique identifier of the game object.
 */
int flux_gameobject_get_id(fluxGameObject obj) {
    LOG_FUNC_CALL();
    assert(obj);
    return obj->id;
}

/**
 * @brief Retrieves the transformation data of a game object.
 * @param obj Pointer to the game object.
 * @return The transformation data as a fluxTransform structure.
 */
fluxTransform flux_gameobject_get_transform(fluxGameObject obj) {
    LOG_FUNC_CALL();
    assert(obj);
    return obj->transform;
}

/**
 * @brief Sets the transformation data for a game object.
 * @param obj Pointer to the game object.
 * @param transform New transformation data.
 */
void flux_gameobject_set_transform(fluxGameObject obj,
                                   fluxTransform transform) {
    LOG_FUNC_CALL();
    assert(obj);
    obj->transform = transform;
}

/**
 * @brief Retrieves the number of scripts attached to a game object.
 * @param obj Pointer to the game object.
 * @return Number of scripts.
 */
int flux_gameobject_get_n_scripts(fluxGameObject obj) {
    LOG_FUNC_CALL();
    assert(obj);
    return obj->n_scripts;
}

/**
 * @brief Checks if the game object functions as a camera.
 * @param obj Pointer to the game object.
 * @return True if the object is a camera, otherwise false.
 */
bool flux_gameobject_is_camera(fluxGameObject obj) {
    LOG_FUNC_CALL();
    assert(obj);
    return obj->is_camera;
}

/**
 * @brief Retrieves the model associated with a game object.
 * @param obj Pointer to the game object.
 * @return The renderModel of the game object.
 */
renderModel flux_gameobject_get_model(fluxGameObject obj) {
    LOG_FUNC_CALL();
    assert(obj);
    return obj->model;
}

/**
 * @brief Checks if the game object has an associated model.
 * @param obj Pointer to the game object.
 * @return True if a model is associated, otherwise false.
 */
bool flux_gameobject_has_model(fluxGameObject obj) {
    LOG_FUNC_CALL();
    assert(obj);
    return obj->model != NULL;
}

/**
 * @brief Retrieves a Camera3D structure initialized based on the game object's
 * properties.
 * @param obj Pointer to the game object configured as a camera.
 * @return A Camera3D structure initialized to the game object's camera
 * settings.
 */
Camera3D flux_gameobject_get_raylib_camera(fluxGameObject obj) {
    LOG_FUNC_CALL();
    assert(obj);
    assert(obj->is_camera);
    Camera3D out;
    out.fovy = obj->fov;
    out.position = obj->transform.pos;
    out.projection = obj->projection;
    out.up = (Vector3){0.0f, 1.0f, 0.0f};
    out.target = Vector3Add(out.position, (Vector3){0.0f, 0.0f, 1.0f});
    CameraPitch(&out, obj->transform.rot.x, true, false, false);
    CameraYaw(&out, obj->transform.rot.y, false);
    CameraRoll(&out, obj->transform.rot.z);
    return out;
}

/**
 * @brief Allocates and initializes a new game object from a prefab and
 * arguments.
 *
 * NOTE: This calls onInit on allocation(!!!)
 *
 * @param id Unique identifier for the new game object.
 * @param transform Initial transformation settings.
 * @param prefab Prefab to initialize the game object from.
 * @param args Additional arguments for initializing scripts.
 * @return Pointer to the newly created game object.
 */
fluxGameObject flux_allocate_gameobject(int id, fluxTransform transform,
                                        fluxPrefab prefab, hstrArray args) {
    LOG_FUNC_CALL();
    fluxGameObject out;
    assert(out = malloc(sizeof(struct fluxGameObjectStruct)));
    out->id = id;
    out->transform = transform;
    out->model = flux_prefab_get_model(prefab);
    out->n_scripts = flux_prefab_get_n_scripts(prefab);
    out->scripts = NULL;
    out->is_camera = flux_prefab_is_camera(prefab);
    out->fov = flux_prefab_get_fov(prefab);
    out->projection = flux_prefab_get_projection(prefab);
    out->visible = true;
    if (out->n_scripts != 0) {
        assert(out->scripts = malloc(sizeof(fluxScript) * out->n_scripts));
        for (int i = 0; i < out->n_scripts; i++) {
            out->scripts[i] =
                flux_allocate_script(flux_prefab_get_scripts(prefab)[i]);
            fluxCallback_onInit(out, out->scripts[i], args);
        }
    }
    return out;
}

/**
 * @brief Retrieves a specific script attached to a game object.
 * @param obj Pointer to the game object.
 * @param i Index of the script to retrieve.
 * @return Pointer to the script.
 */
fluxScript flux_gameobject_get_script(fluxGameObject obj, int i) {
    LOG_FUNC_CALL();
    assert(obj);
    assert(i >= 0);
    assert(i < obj->n_scripts);
    return obj->scripts[i];
}

/**
 * @brief Frees all resources associated with a game object.
 * @param obj Pointer to the game object to destroy.
 */
void flux_destroy_gameobject(fluxGameObject obj) {
    LOG_FUNC_CALL();
    assert(obj);
    if (obj->scripts) {
        assert(obj->n_scripts > 0);
        // scene allocator should free script data?
        free(obj->scripts);
    } else {
        assert(obj->n_scripts == 0);
    }
    free(obj);
}

/**
 * @brief Checks if a game object is set to be visible
 * @param obj Pointer to the game object.
 * @return `true` if the game object is set to be visible, `false` otherwise
 */
bool flux_gameobject_is_visible(fluxGameObject obj){
    return obj->visible;
}

/**
 * @brief Sets the visibility value of a game object
 * @param obj Pointer to the game object.
 * @param visible `true` if the game object should be visible, `false` if not
 */
void flux_gameobject_set_visible(fluxGameObject obj, bool visible){
    obj->visible = visible;
}