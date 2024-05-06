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

static fluxGameObject* game_objects;
static fluxPrefab* prefabs = NULL;
static int n_prefabs = 0;
static int n_objects = 0;
static fluxGameObject active_camera = NULL;

void flux_reset_scene(void) {
    flux_init_scene_allocator();
    n_objects = 0;
    active_camera = NULL;
}

void flux_close_scene(void) {
    if (prefabs){
        for (int i = 0; i < n_prefabs; i++){
            flux_delete_prefab(prefabs[i]);
        }
        free(prefabs);
        prefabs = NULL;
        n_prefabs = 0;
    }
    if (game_objects){
        for (int i = 0; i < n_objects; i++){
            flux_destroy_gameobject(game_objects[i]);
        }
        free(game_objects);
        game_objects = NULL;
        n_objects = 0;
    }
    flux_close_scene_allocator();
}

void flux_load_scene(const char* path) {
    assert(path);
    assert(prefabs == NULL);
    assert(n_prefabs == 0);
    TraceLog(LOG_INFO,"loading scene %s",path);
    fluxParsedScene parsed_scene = parser_read_scene(path);

    active_camera = NULL;

    TraceLog(LOG_INFO,"scene name: %s",hstr_unpack(parser_parsed_scene_get_name(parsed_scene)));

    for (int i = 0; i < parser_parsed_scene_get_n_prefabs(parsed_scene); i++){
        fluxParsedPrefab parsed_prefab = parser_parsed_scene_get_prefab(parsed_scene,i);
        fluxPrefab prefab = flux_load_prefab(parsed_prefab);
        prefabs = realloc(prefabs,sizeof(fluxPrefab) * (n_prefabs + 1));
        prefabs[n_prefabs] = prefab;
        n_prefabs++;
    }

    for (int i = 0; i < parser_parsed_scene_get_n_gameobjects(parsed_scene); i++){
        fluxParsedGameObject parsed_gameobject = parser_parsed_scene_get_gameobject(parsed_scene,i);
        fluxTransform transform = parser_parsed_gameobject_get_transform(parsed_gameobject);
        hstr prefab_name = hstr_incref(parser_parsed_gameobject_get_prefab_name(parsed_gameobject));
        fluxPrefab to_instantiate = NULL;
        for (int j = 0; j < n_prefabs; j++){
            fluxPrefab prefab = prefabs[i];
            if (strcmp(hstr_unpack(flux_prefab_get_name(prefab)),hstr_unpack(prefab_name)) != 0)continue;
            to_instantiate = prefab;
        }

        assert(to_instantiate != NULL);
        hstr_decref(prefab_name);

        fluxGameObject allocated = flux_allocate_gameobject(i,transform,to_instantiate);

        if (flux_gameobject_is_camera(allocated) && (active_camera == NULL)){
            active_camera = allocated;
        }

        game_objects = realloc(game_objects, sizeof(fluxGameObject) * (n_objects + 1));
        game_objects[n_objects] = allocated;
        n_objects++;
    }

    parser_delete_parsed_scene(parsed_scene);
}

void flux_draw_scene(void) {
    if (!active_camera)
        return;

    for (int i = 0; i < n_prefabs; i++){
        if (flux_prefab_is_camera(prefabs[i]))continue;
        if (flux_prefab_get_model(prefabs[i]) == NULL)continue;
        render_reset_instances(flux_prefab_get_model(prefabs[i]));
    }

    for (int i = 0; i < n_objects; i++){
        fluxGameObject obj = game_objects[i];
        if (flux_gameobject_is_camera(obj))continue;
        if (!flux_gameobject_has_model(obj))continue;
        render_add_model_instance(flux_gameobject_get_model(obj),flux_gameobject_get_transform(obj));
    }

    Camera3D cam = flux_gameobject_get_raylib_camera(active_camera);

    render_begin(cam);

    for (int i = 0; i < n_prefabs; i++){
        if (flux_prefab_is_camera(prefabs[i]))continue;
        if (flux_prefab_get_model(prefabs[i]) == NULL)continue;
        render_rmodel(flux_prefab_get_model(prefabs[i]),WHITE);
    }

    render_calculate_shadows();

    render_end();

    //TraceLog(LOG_INFO,"camera found");
    /*Camera3D cam = flux_gameobject_get_raylib_camera(active_camera);
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

    EndMode3D();*/
}