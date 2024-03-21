#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "config.h"
#include "transform.h"
#include "gameobject.h"
#include "sceneallocator.h"

// game object struct
struct fluxGameObjectStruct{
    // unique ID of gameobject
    int id;
    // name of the prefab that this is an instance of
    const char* name;
    // the tag of the gameobject
    const char* tag;
    // the transform of the object
    struct fluxTransform transform;
    // the model of the object, if `has_model`
    bool has_model;
    Model model;
    Color tint;
    // the camera of the object, if `!has_model` and `is_camera`
    bool is_camera;
    Camera3D cam;
    // scripts associated with this gameobject
    int n_scripts;
    fluxScript scripts[FLUX_MAX_SCRIPTS];
    // gameobjects children
    int n_children;
    struct fluxGameObjectStruct* children[FLUX_MAX_CHILDREN];
};

fluxGameObject flux_allocate_gameobject(const char* name, const char* tag, fluxTransform transform){
    fluxGameObject out = (fluxGameObject)flux_scene_alloc(sizeof(struct fluxGameObjectStruct));
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

void flux_gameobject_set_model(fluxGameObject obj, Model model, Color tint){
    assert((obj != NULL) && "tried to set model of NULL game object!");
    assert((!(obj->is_camera)) && "tried to add model to camera!");
    obj->has_model = 1;
    obj->model = model;
}

void flux_gameobject_set_camera(fluxGameObject obj, float fovy, int projection){
    assert((obj != NULL) && "tried to set camera of NULL game object!");
    assert((!(obj->has_model)) && "tried to set camera of model!");
    obj->is_camera = 1;
    obj->cam.fovy = fovy;
    obj->cam.projection = projection;
}

void flux_gameobject_add_script(fluxGameObject obj, enum fluxScriptID script){
    assert((obj != NULL) && "tried to add script to NULL game object!");
    assert((obj->n_scripts < FLUX_MAX_SCRIPTS) && "game object already has FLUX_MAX_SCRIPTS scripts");
    obj->scripts[obj->n_scripts] = flux_allocate_script(script);
    obj->n_scripts++;
}

void flux_gameobject_add_child(fluxGameObject obj, fluxGameObject child){
    assert((obj != NULL) && "tried to add child to NULL game object!");
    assert((child != NULL) && "tried to add NULL child!");
    assert((obj->n_children < FLUX_MAX_CHILDREN) && "game object already has FLUX_MAX_CHILDREN scripts");
    obj->children[obj->n_children] = child;
    obj->n_children++;
}

bool flux_gameobject_is_camera(fluxGameObject obj){
    assert(obj);
    return obj->is_camera;
}

Camera3D flux_gameobject_get_raylib_camera(fluxGameObject obj){
    assert(flux_gameobject_is_camera(obj));
    Camera3D out;
    out.fovy = obj->cam.fovy;
    out.position = obj->transform.pos;
    out.projection = obj->cam.projection;
    out.target = Vector3Add(out.position,(Vector3){1.0f,0.0f,0.0f});
    CameraPitch(&out, obj->transform.rot.x,true,false,false);
    CameraYaw(&out, obj->transform.rot.y,false);
    CameraRoll(&out, obj->transform.rot.z);
    return out;
}