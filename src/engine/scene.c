#include "config.h"
#include "gameobject.h"
#include "hqtools/hqtools.h"
#include "prefabs.h"
#include "raylib.h"
#include "raymath.h"
#include "sceneallocator.h"
#include "scripts.h"
#include "transform.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static fluxGameObject game_objects[FLUX_MAX_GAMEOBJECTS];
static int n_objects = 0;
static fluxGameObject active_camera;

void flux_reset_scene(void) {
    flux_init_scene_allocator();
    n_objects = 0;
    active_camera = NULL;
}

void flux_close_scene(void) { flux_close_scene_allocator(); }

void flux_scene_instantiate_prefab(enum fluxPrefabID prefab,
                                   fluxTransform transform) {
    FLUX_ASSERT(n_objects < FLUX_MAX_GAMEOBJECTS,
                "FLUX<scene.c>: too many objects instantiated (%d >= %d)",
                n_objects, FLUX_MAX_GAMEOBJECTS);
    fluxGameObject obj = flux_instantiate_prefab(prefab, transform);
    game_objects[n_objects] = obj;
    n_objects++;
    if (flux_gameobject_is_camera(obj) && (active_camera == NULL)) {
        TraceLog(LOG_INFO,
                 "FLUX<scene.c>: setting scene active camera to gameobject "
                 "with id %d",
                 flux_gameobject_get_id(obj));
        active_camera = obj;
    }
}

void flux_draw_scene(void) {
    if (!active_camera)
        return;
    Camera3D cam = flux_gameobject_get_raylib_camera(active_camera);
    TraceLog(LOG_DEBUG,
             "FLUX<scene.c>: drawing scene with camera: position = {%g %g %g}, "
             "target = {%g %g %g}, up = {%g %g %g}, fovy = %g, projection = %d",
             cam.position.x, cam.position.y, cam.position.z, cam.target.x,
             cam.target.y, cam.target.z, cam.up.x, cam.up.y, cam.up.z, cam.fovy,
             cam.projection);
    BeginMode3D(cam);
    DrawGrid(100, 0.1);

    for (int i = 0; i < n_objects; i++) {
        flux_gameobject_draw(game_objects[i]);
    }

    EndMode3D();
}