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
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "sceneallocator.h"
#include "transform.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Represents a game object within the scene.
 * Game objects can have models, cameras, scripts, and child objects.
 */
struct fluxGameObjectStruct {
    int id;           ///< Unique ID of the game object.
    const char* name; ///< Name of the prefab instance.
    const char* tag;  ///< Tag of the game object for categorization.
    struct fluxTransform
        transform;  ///< Transform of the game object in the scene.
    bool has_model; ///< Indicates if the game object has a model.
    Model model;    ///< Model of the game object.
    Color tint;     ///< Tint color of the model.
    bool is_camera; ///< Indicates if the game object is a camera.
    Camera3D cam;   ///< Camera properties if the game object is a camera.
    int n_scripts;  ///< Number of scripts attached to the game object.
    fluxScript
        scripts[FLUX_MAX_SCRIPTS]; ///< Scripts attached to the game object.
    int n_children;                ///< Number of child objects.
    struct fluxGameObjectStruct*
        children[FLUX_MAX_CHILDREN]; ///< Child objects.
};

/**
 * Allocates and initializes a new game object.
 *
 * @param name Name of the new game object.
 * @param tag Tag for the new game object.
 * @param transform Initial transform for the new game object.
 * @return A pointer to the newly allocated game object.
 */
fluxGameObject flux_allocate_gameobject(const char* name, const char* tag,
                                        fluxTransform transform) {
    fluxGameObject out =
        (fluxGameObject)flux_scene_alloc(sizeof(struct fluxGameObjectStruct));
    out->id = flux_scene_get_unique_id();
    out->name = name;
    out->tag = tag;
    out->transform = transform;
    out->has_model = 0;
    out->is_camera = 0;
    out->n_children = 0;
    out->n_scripts = 0;
    return out;
}

/**
 * Sets the model and tint for a game object.
 *
 * @param obj Pointer to the game object.
 * @param model Model to set for the game object.
 * @param tint Tint color to apply to the model.
 */
void flux_gameobject_set_model(fluxGameObject obj, Model model, Color tint) {
    FLUX_ASSERT((obj != NULL),
                "FLUX<gameobject.c>: tried to set model of NULL game object!");
    FLUX_ASSERT(!(obj->is_camera),
                "FLUX<gameobject.c>: tried to add model to camera!");
    obj->has_model = 1;
    obj->model = model;
    obj->tint = tint;
}

/**
 * Configures a game object as a camera.
 *
 * @param obj Pointer to the game object.
 * @param fovy Field of view angle in the y direction, in degrees.
 * @param projection Projection type of the camera.
 */
void flux_gameobject_set_camera(fluxGameObject obj, float fovy,
                                int projection) {
    FLUX_ASSERT(obj != NULL,
                "FLUX<gameobject.c>: tried to set camera of NULL game object!");
    FLUX_ASSERT(!obj->has_model,
                "FLUX<gameobject.c>: tried to set camera of model!");
    obj->is_camera = 1;
    obj->cam.fovy = fovy;
    obj->cam.projection = projection;
}

/**
 * Adds a script to a game object.
 *
 * @param obj Pointer to the game object.
 * @param script ID of the script to add.
 */
void flux_gameobject_add_script(fluxGameObject obj, enum fluxScriptID script) {
    FLUX_ASSERT(obj != NULL,
                "FLUX<gameobject.c>: tried to add script to NULL game object!");
    FLUX_ASSERT(
        obj->n_scripts < FLUX_MAX_SCRIPTS,
        "FLUX<gameobject.c>: game object already has FLUX_MAX_SCRIPTS scripts");
    obj->scripts[obj->n_scripts] = flux_allocate_script(script);
    obj->n_scripts++;
}

/**
 * Adds a child game object.
 *
 * @param obj Pointer to the parent game object.
 * @param child Pointer to the child game object to add.
 */
void flux_gameobject_add_child(fluxGameObject obj, fluxGameObject child) {
    FLUX_ASSERT(obj != NULL,
                "FLUX<gameobject.c>: tried to add child to NULL game object!");
    FLUX_ASSERT(child != NULL, "FLUX<gameobject.c>: tried to add NULL child!");
    FLUX_ASSERT(obj->n_children < FLUX_MAX_CHILDREN,
                "FLUX<gameobject.c>: game object already has FLUX_MAX_CHILDREN "
                "scripts");
    obj->children[obj->n_children] = child;
    obj->n_children++;
}

/**
 * Checks if a game object is configured as a camera.
 *
 * @param obj Pointer to the game object.
 * @return True if the game object is a camera, otherwise false.
 */
bool flux_gameobject_is_camera(fluxGameObject obj) {
    assert(obj);
    return obj->is_camera;
}

/**
 * Checks if a game object has a model.
 *
 * @param obj Pointer to the game object.
 * @return True if the game object has a model, otherwise false.
 */
bool flux_gameobject_has_model(fluxGameObject obj) {
    assert(obj);
    return obj->has_model;
}

/**
 * Retrieves the model of a game object.
 *
 * @param obj Pointer to the game object.
 * @return Model of the game object.
 */
Model flux_gameobject_get_model(fluxGameObject obj) {
    assert(flux_gameobject_has_model(obj));
    return obj->model;
}

/**
 * Retrieves the transform of a game object.
 *
 * @param obj Pointer to the game object.
 * @return Transform of the game object.
 */
fluxTransform flux_gameobject_get_transform(fluxGameObject obj) {
    assert(obj);
    return obj->transform;
}

/**
 * Retrieves the tint color of a game object's model.
 *
 * @param obj Pointer to the game object.
 * @return Tint color of the model.
 */
Color flux_gameobject_get_tint(fluxGameObject obj) {
    assert(obj);
    return obj->tint;
}

/**
 * Retrieves the unique ID of a game object.
 *
 * @param obj Pointer to the game object.
 * @return Unique ID of the game object.
 */
int flux_gameobject_get_id(fluxGameObject obj) {
    assert(obj);
    return obj->id;
}

/**
 * Retrieves the raylib camera configuration of a game object.
 * This function is applicable only if the game object is configured as
 * a camera.
 *
 * @param obj Pointer to the game object.
 * @return A raylib Camera3D structure representing the camera's configuration.
 */
Camera3D flux_gameobject_get_raylib_camera(fluxGameObject obj) {
    FLUX_ASSERT(flux_gameobject_is_camera(obj),
                "FLUX<gameobject.c>: tried to get camera of game object that "
                "isn't camera");
    Camera3D out;
    out.fovy = obj->cam.fovy;
    out.position = obj->transform.pos;
    out.projection = obj->cam.projection;
    out.up = (Vector3){0.0f, 1.0f, 0.0f};
    out.target = Vector3Add(out.position, (Vector3){0.0f, 0.0f, 1.0f});
    CameraPitch(&out, obj->transform.rot.x, true, false, false);
    CameraYaw(&out, obj->transform.rot.y, false);
    CameraRoll(&out, obj->transform.rot.z);
    return out;
}

void flux_gameobject_draw(fluxGameObject obj) {
    assert(obj);

    if ((!flux_gameobject_is_camera(obj)) && (flux_gameobject_has_model(obj))) {
        // TraceLog(LOG_INFO,"drawing model");
        Model model = flux_gameobject_get_model(obj);
        fluxTransform transform = flux_gameobject_get_transform(obj);
        Color tint = flux_gameobject_get_tint(obj);
        Quaternion qrot =
            QuaternionFromEuler(Wrap(transform.rot.x, 0, 2 * M_PI),
                                Wrap(transform.rot.y, 0, 2 * M_PI),
                                Wrap(transform.rot.z, 0, 2 * M_PI));
        Vector3 rot_axis;
        float rot_amount;
        QuaternionToAxisAngle(qrot, &rot_axis, &rot_amount);
        DrawModelEx(model, transform.pos, rot_axis, rot_amount, transform.scale,
                    tint);
    }
}