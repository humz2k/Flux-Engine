#include "raylib.h"
#include "raymath.h"
#include "config.h"
#include "transform.h"
#include "gameobject.h"

struct fluxScriptStruct;
typedef struct fluxScriptStruct* fluxScript;

// game object struct
struct fluxGameObjectStruct{
    // unique ID of gameobject
    int id;
    // the tag of the gameobject
    const char* tag;
    // the transform of the object
    struct fluxTransform transform;
    // the model of the object, if `has_model`
    bool has_model;
    Model model;
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

/*enum fluxScriptID{
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

};*/