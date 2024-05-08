/**
 * @file scene.c
 * @brief Manages the lifecycle of scenes in the game, including loading, updating, drawing, and closing scenes.
 *
 * This file provides functionality to handle game scenes, which includes managing game objects, prefabs, and scripts,
 * as well as handling scene-specific callbacks and rendering processes. It supports operations such as scene loading,
 * resetting, and rendering, and incorporates scene-specific data handling.
 */

#include "scene.h"
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

static fluxGameObject* game_objects; ///< Array of game objects currently in the scene.
static fluxPrefab* prefabs = NULL; ///< Array of prefabs used in the scene.
static int n_prefabs = 0; ///< Count of prefabs loaded into the scene.
static int n_objects = 0; ///< Count of game objects currently in the scene.
static fluxGameObject active_camera = NULL; ///< Active camera game object.

/**
 * @brief Resets the scene to its initial state.
 *
 * Initializes the scene allocator and resets scene-specific data such as active camera and object counts.
 */
void flux_reset_scene(void) {
    LOG_FUNC_CALL();
    flux_init_scene_allocator();
    n_objects = 0;
    active_camera = NULL;
}

/**
 * @brief Closes the scene, freeing resources and cleaning up.
 *
 * This function handles the cleanup and memory management for all game objects and prefabs within the scene, and closes the scene allocator.
 */
void flux_close_scene(void) {
    LOG_FUNC_CALL();
    flux_scene_script_callback(ONDESTROY);
    if (prefabs) {
        for (int i = 0; i < n_prefabs; i++) {
            flux_delete_prefab(prefabs[i]);
        }
        free(prefabs);
        prefabs = NULL;
        n_prefabs = 0;
    }
    if (game_objects) {
        for (int i = 0; i < n_objects; i++) {
            flux_destroy_gameobject(game_objects[i]);
        }
        free(game_objects);
        game_objects = NULL;
        n_objects = 0;
    }
    flux_close_scene_allocator();
}

/**
 * @brief Instantiates a prefab in the current scene
 *
 * @param prefab the prefab to instantiate
 * @param transform transform
 * @param args extra args passed to onInit
 */
void flux_instantiate_prefab(fluxPrefab prefab, fluxTransform transform, hstrArray args){
    int id = n_objects;
    fluxGameObject allocated = flux_allocate_gameobject(
            id, transform, prefab,
            args);
    if (flux_gameobject_is_camera(allocated) && (active_camera == NULL)) {
        active_camera = allocated;
    }
    game_objects =
            realloc(game_objects, sizeof(fluxGameObject) * (n_objects + 1));
    game_objects[n_objects] = allocated;
    n_objects++;
}

/**
 * @brief Instantiates a prefab in the current scene by name
 *
 * @param name the name of the prefab
 * @param transform transform
 * @param args extra args passed to onInit
 */
void flux_instantiate_prefab_by_name(const char* name, fluxTransform transform, hstrArray args){
    fluxPrefab to_instantiate = NULL;
    for (int j = 0; j < n_prefabs; j++) {
        fluxPrefab prefab = prefabs[j];
        if (strcmp(hstr_unpack(flux_prefab_get_name(prefab)),
                    name) != 0)
            continue;
        to_instantiate = prefab;
    }

    assert(to_instantiate != NULL);

    flux_instantiate_prefab(to_instantiate,transform,args);
}

/**
 * @brief Loads a scene from a specified path.
 *
 * Parses a scene file to construct the scene's structure in memory, including creating game objects and prefabs based on parsed data.
 * @param path The file path of the scene to load.
 */
void flux_load_scene(const char* path) {
    LOG_FUNC_CALL();
    assert(path);
    assert(prefabs == NULL);
    assert(n_prefabs == 0);
    TraceLog(LOG_INFO, "loading scene %s", path);
    fluxParsedScene parsed_scene = parser_read_scene(path);

    active_camera = NULL;

    TraceLog(LOG_INFO, "scene name: %s",
             hstr_unpack(parser_parsed_scene_get_name(parsed_scene)));

    for (int i = 0; i < parser_parsed_scene_get_n_prefabs(parsed_scene); i++) {
        fluxParsedPrefab parsed_prefab =
            parser_parsed_scene_get_prefab(parsed_scene, i);
        fluxPrefab prefab = flux_load_prefab(parsed_prefab);
        prefabs = realloc(prefabs, sizeof(fluxPrefab) * (n_prefabs + 1));
        prefabs[n_prefabs] = prefab;
        n_prefabs++;
    }

    for (int i = 0; i < parser_parsed_scene_get_n_gameobjects(parsed_scene);
         i++) {
        fluxParsedGameObject parsed_gameobject =
            parser_parsed_scene_get_gameobject(parsed_scene, i);

        fluxTransform transform =
            parser_parsed_gameobject_get_transform(parsed_gameobject);

        flux_instantiate_prefab_by_name(
                hstr_unpack(parser_parsed_gameobject_get_prefab_name(parsed_gameobject)),
                transform,
                parser_parsed_gameobject_get_args(parsed_gameobject));


    }

    parser_delete_parsed_scene(parsed_scene);
}

/**
 * @brief Executes a specific script callback for all scripts attached to all game objects in the scene.
 *
 * Iterates over all game objects and their attached scripts to execute a given callback type.
 * @param callback Type of script callback to execute (update, draw, etc.).
 */
void flux_scene_script_callback(script_callback_t callback) {
    LOG_FUNC_CALL();
    void (*func)(fluxGameObject, fluxScript);
    switch (callback) {
    case ONUPDATE:
        func = fluxCallback_onUpdate;
        break;
    case AFTERUPDATE:
        func = fluxCallback_afterUpdate;
        break;
    case ONDESTROY:
        func = fluxCallback_onDestroy;
        break;
    case ONDRAW:
        func = fluxCallback_onDraw;
        break;
    case ONDRAW2D:
        func = fluxCallback_onDraw2D;
        break;
    }

    for (int i = 0; i < n_objects; i++) {
        fluxGameObject obj = game_objects[i];
        for (int j = 0; j < flux_gameobject_get_n_scripts(obj); j++) {
            fluxScript script = flux_gameobject_get_script(obj, j);
            func(obj, script);
        }
    }
}

/**
 * @brief Executes a the signal script callback for all scripts attached to all game objects in the scene.
 *
 * Iterates over all game objects and their attached scripts to execute the signal callback.
 * @param signal the signal
 */
void flux_scene_signal_handler(int signal){
    LOG_FUNC_CALL();
    for (int i = 0; i < n_objects; i++) {
        fluxGameObject obj = game_objects[i];
        for (int j = 0; j < flux_gameobject_get_n_scripts(obj); j++) {
            fluxScript script = flux_gameobject_get_script(obj, j);
            fluxCallback_onSignal(obj, script, signal);
        }
    }
}

/**
 * @brief Draws the entire scene.
 *
 * This function manages the drawing of all renderable objects within the scene, handles shadow calculation, and triggers rendering-related callbacks.
 */
void flux_draw_scene(void) {
    LOG_FUNC_CALL();
    if (!active_camera)
        return;

    for (int i = 0; i < n_prefabs; i++) {
        if (flux_prefab_is_camera(prefabs[i]))
            continue;
        if (flux_prefab_get_model(prefabs[i]) == NULL)
            continue;
        render_reset_instances(flux_prefab_get_model(prefabs[i]));
    }

    for (int i = 0; i < n_objects; i++) {
        fluxGameObject obj = game_objects[i];
        if (flux_gameobject_is_camera(obj))
            continue;
        if (!flux_gameobject_has_model(obj))
            continue;
        render_add_model_instance(flux_gameobject_get_model(obj),
                                  flux_gameobject_get_transform(obj));
    }

    Camera3D cam = flux_gameobject_get_raylib_camera(active_camera);

    render_begin(cam);

    for (int i = 0; i < n_prefabs; i++) {
        if (flux_prefab_is_camera(prefabs[i]))
            continue;
        if (flux_prefab_get_model(prefabs[i]) == NULL)
            continue;
        render_rmodel(flux_prefab_get_model(prefabs[i]), WHITE);
    }

    render_calculate_shadows();

    render_end();

    flux_scene_script_callback(ONDRAW2D);

}