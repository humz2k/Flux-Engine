#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "raylib.h"
#include "raymath.h"
#include "config.h"
#include "transform.h"
#include "gameobject.h"
#include "prefabs.h"
#include "scripts.h"
#include "sceneallocator.h"

static fluxGameObject game_objects[FLUX_MAX_GAMEOBJECTS];
static int n_objects = 0;
static fluxGameObject active_camera;

void flux_reset_scene(void){
    flux_init_scene_allocator();
    n_objects = 0;
    active_camera = NULL;
}

void flux_close_scene(void){
    flux_close_scene_allocator();
}

void flux_scene_instantiate_prefab(enum fluxPrefabID prefab, fluxTransform transform){
    assert(n_objects < FLUX_MAX_GAMEOBJECTS);
    fluxGameObject obj = flux_instantiate_prefab(prefab, transform);
    game_objects[n_objects] = obj;
    n_objects++;
    if (flux_gameobject_is_camera(obj) && (active_camera == NULL)){
        TraceLog(LOG_INFO,"setting scene active camera");
        active_camera = obj;
    }
}

void flux_draw_scene(void){
    if (!active_camera)return;
    Camera3D cam = flux_gameobject_get_raylib_camera(active_camera);
    BeginMode3D(cam);
    DrawGrid(100,0.1);
    EndMode3D();
}