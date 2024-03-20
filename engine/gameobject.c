#include "raylib.h"
#include "raymath.h"
#include "config.h"
#include "transform.h"
#include "gameobject.h"

enum fluxScriptID{
    RAND,WHO,CARES
};

struct fluxGameObjectPrefabStruct{
    // the name of the prefab
    const char* name;

    // model associated with prefab if `has_model`
    bool has_model;
    const char* model_path;

    // whether the object is visible on instantiation
    bool visible;

    // scripts to instantiate
    int n_scripts;
    enum fluxScriptID scripts[FLUX_MAX_SCRIPTS];

    // children to instantiate
    int n_children;
    struct fluxGameObjectPrefabStruct* children[FLUX_MAX_CHILDREN];

    // transform to instantiate
    struct fluxTransform transform;

};