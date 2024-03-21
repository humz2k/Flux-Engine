/**
 * @file prefabs.c
 * @brief Prefab management for a game engine using raylib.
 *
 * This file provides the functionality to manage game prefabs, including registration,
 * instantiation, and cleanup of prefabs. It integrates with raylib for rendering and supports
 * game objects with models, cameras, scripts, and hierarchical children.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "raylib.h"
#include "raymath.h"
#include "config.h"
#include "prefabs.h"
#include "transform.h"
#include "gameobject.h"
#include "game_callbacks.h"
#include "scripts.h"

/**
 * A struct that defines a game prefab.
 * Prefabs are templates from which game objects can be instantiated.
 */
struct fluxPrefabStruct {
    const char* name;               ///< Name of the prefab.
    const char* tag;                ///< Tag for categorizing or identifying.
    const char* model_path;         ///< Path to the 3D model file.
    bool is_camera;                 ///< Indicates if this prefab is a camera.
    int n_scripts;                  ///< Number of scripts attached to this prefab.
    enum fluxScriptID scripts[FLUX_MAX_SCRIPTS]; ///< Array of script IDs attached to the prefab.
    int n_children;                 ///< Number of child prefabs.
    enum fluxPrefabID children[FLUX_MAX_CHILDREN]; ///< Array of child prefab IDs.
};

static int n_prefabs = 0;                       ///< Number of registered prefabs.
static int allocated_prefab_space = 0;          ///< Allocated space for prefabs array.
static struct fluxPrefabStruct* allocated_prefabs = NULL; ///< Dynamic array of prefabs.

/**
 * Cleans up and deallocates memory used by prefabs.
 * This function should be called at the end of the program to ensure proper memory management.
 */
void flux_close_prefabs(void) {
    TraceLog(LOG_INFO, "FLUX<prefabs.c>: flux_close_prefabs called (deleting %d prefabs)", n_prefabs);
    free(allocated_prefabs);
}

/**
 * Registers a new prefab with specified properties.
 *
 * @param name Name of the prefab.
 * @param tag Tag for the prefab.
 * @param model_path Path to the model file, NULL if it's not a model.
 * @param is_camera Flag indicating if the prefab is a camera.
 * @param n_scripts Number of scripts attached to the prefab.
 * @param scripts Array of script IDs.
 * @param n_children Number of child prefabs.
 * @param children Array of child prefab IDs.
 */
void flux_register_prefab(const char* name, const char* tag, const char* model_path, int is_camera, int n_scripts, enum fluxScriptID* scripts, int n_children, enum fluxPrefabID* children){
    FLUX_ASSERT((!(model_path && is_camera)), "FLUX<prefabs.c>: a prefab can't be a model and a camera");
    FLUX_ASSERT((n_scripts <= FLUX_MAX_SCRIPTS), "FLUX<prefabs.c>: tried to add too many scripts (%d > %d) to game object", n_scripts, FLUX_MAX_SCRIPTS);
    FLUX_ASSERT((n_children <= FLUX_MAX_CHILDREN), "FLUX<prefabs.c>: tried to add too many children (%d > %d) to game object", n_children, FLUX_MAX_CHILDREN);
    if (n_prefabs >= allocated_prefab_space){
        allocated_prefab_space *= 2;
        assert(allocated_prefabs = realloc(allocated_prefabs,sizeof(struct fluxPrefabStruct) * allocated_prefab_space));
    }
    FLUX_ASSERT(n_prefabs < allocated_prefab_space, "FLUX<prefabs.c>: for some reason n_prefabs (%d) >= allocated_prefab_space (%d)",n_prefabs,allocated_prefab_space);
    struct fluxPrefabStruct* prefab = &allocated_prefabs[n_prefabs];
    n_prefabs++;
    prefab->name = name;
    prefab->tag = tag;
    prefab->model_path = model_path;
    prefab->is_camera = is_camera;
    prefab->n_scripts = n_scripts;
    for (int i = 0; i < n_scripts; i++){
        prefab->scripts[i] = scripts[i];
    }
    prefab->n_children = n_children;
    for (int i = 0; i < n_children; i++){
        FLUX_ASSERT(children[i] < n_prefabs, "FLUX<prefabs.c>: prefabID %d is out of bounds (%d)",(int)children[i],n_prefabs);
        prefab->children[i] = children[i];
    }

}

#define FLUX_PRIVATE_PREFABS
#include "GENERATED_PREFABS.h"

/**
 * Initializes the prefab system.
 * Allocates initial memory for prefabs and registers a cleanup callback.
 */
void flux_init_prefabs(void){
    TraceLog(LOG_INFO,"FLUX<prefabs.c>: flux_init_prefabs called");
    n_prefabs = 0;
    allocated_prefab_space = 10;
    assert(allocated_prefabs = malloc(sizeof(struct fluxPrefabStruct) * allocated_prefab_space));
    flux_register_callback(fluxGameCallback_onGameClose,flux_close_prefabs);

    init_all_prefabs();

    TraceLog(LOG_INFO,"FLUX<prefabs.c>: initialized %d prefabs",n_prefabs);
}

/**
 * Instantiates a prefab as a game object.
 *
 * @param prefabid ID of the prefab to instantiate.
 * @param transform Initial transform for the instantiated game object.
 * @return A new game object instance based on the specified prefab.
 */
fluxGameObject flux_instantiate_prefab(enum fluxPrefabID prefabid, fluxTransform transform){
    FLUX_ASSERT(prefabid < n_prefabs,"FLUX<prefabs.c>: prefabID %d is out of bounds (%d)",prefabid,n_prefabs);
    struct fluxPrefabStruct prefab = allocated_prefabs[prefabid];
    fluxGameObject out = flux_allocate_gameobject(prefab.name,prefab.tag,transform);
    if (prefab.model_path != NULL){
        Model model = flux_scene_load_model(prefab.model_path);
        flux_gameobject_set_model(out,model,WHITE);
    }
    if (prefab.is_camera){
        flux_gameobject_set_camera(out,45,CAMERA_PERSPECTIVE);
    }
    for (int i = 0; i < prefab.n_scripts; i++){
        flux_gameobject_add_script(out,prefab.scripts[i]);
    }
    for (int i = 0; i < prefab.n_children; i++){
        flux_gameobject_add_child(out,flux_instantiate_prefab(prefab.children[i],transform));
    }
    return out;
}