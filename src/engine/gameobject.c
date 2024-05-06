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
#include "pipeline.h"
#include "prefabs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Represents a game object within the scene.
 * Game objects can have models, cameras, scripts, and child objects.
 */
struct fluxGameObjectStruct {
    int id;           ///< Unique ID of the game object.
    fluxTransform transform;
    renderModel model;
    int n_scripts;
    fluxScript* scripts;
    bool is_camera;
    float fov;
    int projection;
};

int flux_gameobject_get_id(fluxGameObject obj){
    assert(obj);
    return obj->id;
}

fluxTransform flux_gameobject_get_transform(fluxGameObject obj){
    assert(obj);
    return obj->transform;
}

void flux_gameobject_set_transform(fluxGameObject obj, fluxTransform transform){
    assert(obj);
    obj->transform = transform;
}

int flux_gameobject_get_n_scripts(fluxGameObject obj){
    assert(obj);
    return obj->n_scripts;
}

bool flux_gameobject_is_camera(fluxGameObject obj){
    assert(obj);
    return obj->is_camera;
}

renderModel flux_gameobject_get_model(fluxGameObject obj){
    assert(obj);
    return obj->model;
}

bool flux_gameobject_has_model(fluxGameObject obj){
    assert(obj);
    return obj->model != NULL;
}

Camera3D flux_gameobject_get_raylib_camera(fluxGameObject obj){
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

fluxGameObject flux_allocate_gameobject(int id, fluxTransform transform, fluxPrefab prefab, hstrArray args){
    fluxGameObject out;
    assert(out = malloc(sizeof(struct fluxGameObjectStruct)));
    out->id = id;
    out->transform = transform;
    out->model = flux_prefab_get_model(prefab);
    out->n_scripts = flux_prefab_get_n_scripts(prefab);
    out->scripts = NULL;
    out->is_camera = flux_prefab_is_camera(prefab);
    if (out->n_scripts != 0){
        assert(out->scripts = malloc(sizeof(fluxScript) * out->n_scripts));
        for (int i = 0; i < out->n_scripts; i++){
            out->scripts[i] = flux_allocate_script(flux_prefab_get_scripts(prefab)[i]);
            fluxCallback_onInit(out,out->scripts[i],args);
        }
    }
    out->fov = flux_prefab_get_fov(prefab);
    out->projection = flux_prefab_get_projection(prefab);
    return out;
}

fluxScript flux_gameobject_get_script(fluxGameObject obj, int i){
    assert(obj);
    assert(i >= 0);
    assert(i < obj->n_scripts);
    return obj->scripts[i];
}

void flux_destroy_gameobject(fluxGameObject obj){
    assert(obj);
    if (obj->scripts){
        assert(obj->n_scripts > 0);
        // scene allocator should free script data?
        free(obj->scripts);
    } else {
        assert(obj->n_scripts == 0);
    }
    free(obj);
}