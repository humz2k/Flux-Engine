#include "config.h"
#include "gameobject.h"
#include "hqtools/hqtools.h"
#include "prefab_parser.h"
#include "prefabs.h"
#include "raylib.h"
#include "raymath.h"
#include "scene_parser.h"
#include "sceneallocator.h"
#include "scripts.h"
#include "transform.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static fluxGameObject game_objects[FLUX_MAX_GAMEOBJECTS];
static fluxPrefab prefabs[FLUX_MAX_GAMEOBJECTS];
static int n_prefabs = 0;
static int n_objects = 0;
static fluxGameObject active_camera;

void flux_reset_scene(void) {
    flux_init_scene_allocator();
    n_objects = 0;
    active_camera = NULL;
}

void flux_close_scene(void) { flux_close_scene_allocator(); }

void flux_load_scene(const char* path) {
    // fluxParsedScene parsed_scene =
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
        // flux_gameobject_draw(game_objects[i]);
    }

    EndMode3D();
}