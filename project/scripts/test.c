#define SCRIPT test
#include "fluxScript.h"

script_data{
    fluxTransform transform;
};

fluxCallback onInit(fluxGameObject obj, script_data* data, hstrArray args){
    data->transform = flux_gameobject_get_transform(obj);
}

fluxCallback onUpdate(fluxGameObject obj, script_data* data){
    //TraceLog(LOG_INFO,"UPDATE...");
    if (IsKeyDown(KEY_A))
        data->transform.pos.x += GetFrameTime();
    if (IsKeyDown(KEY_D))
        data->transform.pos.x -= GetFrameTime();
    flux_gameobject_set_transform(obj,data->transform);
}