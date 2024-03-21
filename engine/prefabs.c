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

struct fluxPrefabStruct{
    const char* name;
    const char* tag;
    const char* model_path;
    bool is_camera;
    int n_scripts;
    enum fluxScriptID scripts[FLUX_MAX_SCRIPTS];
    int n_children;
    enum fluxPrefabID children[FLUX_MAX_CHILDREN];
};

static int n_prefabs = 0;
static int allocated_prefab_space = 0;
static struct fluxPrefabStruct* allocated_prefabs = NULL;

void flux_close_prefabs(void){
    TraceLog(LOG_INFO,"flux_close_prefabs called (deleting %d prefabs)",n_prefabs);
    free(allocated_prefabs);
}

void flux_register_prefab(const char* name, const char* tag, const char* model_path, int is_camera, int n_scripts, enum fluxScriptID* scripts, int n_children, enum fluxPrefabID* children){
    assert((!(model_path && is_camera)) && "a prefab can't be a model and a camera");
    assert((n_scripts <= FLUX_MAX_SCRIPTS));
    assert((n_children <= FLUX_MAX_CHILDREN));
    if (n_prefabs >= allocated_prefab_space){
        allocated_prefab_space *= 2;
        assert(allocated_prefabs = realloc(allocated_prefabs,sizeof(struct fluxPrefabStruct) * allocated_prefab_space));
    }
    assert(n_prefabs < allocated_prefab_space);
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
        assert(children[i] < n_prefabs);
        prefab->children[i] = children[i];
    }

}

#define FLUX_PRIVATE_PREFABS
#include "GENERATED_PREFABS.h"

void flux_init_prefabs(void){
    TraceLog(LOG_INFO,"flux_init_prefabs called");
    n_prefabs = 0;
    allocated_prefab_space = 10;
    assert(allocated_prefabs = malloc(sizeof(struct fluxPrefabStruct) * allocated_prefab_space));
    flux_register_callback(fluxGameCallback_onGameClose,flux_close_prefabs);

    init_all_prefabs();

    TraceLog(LOG_INFO,"initialized %d prefabs",n_prefabs);
}

fluxGameObject flux_instantiate_prefab(enum fluxPrefabID prefabid, fluxTransform transform){
    assert(prefabid < n_prefabs);
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