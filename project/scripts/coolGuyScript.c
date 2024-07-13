#define SCRIPT coolGuyScript
#include "fluxScript.h"

script_data {  };

fluxCallback onInit(fluxGameObject obj, script_data* data, hstrArray args) {

}

fluxCallback onUpdate(fluxGameObject obj, script_data* data) {
    fluxTransform transform = flux_gameobject_get_transform(obj);
    transform.rot.y += GetFrameTime();
    flux_gameobject_set_transform(obj,transform);
}

fluxCallback afterUpdate(fluxGameObject obj, script_data* data) {}

fluxCallback onDestroy(fluxGameObject obj, script_data* data) {}