#define SCRIPT test2
#include "fluxScript.h"

script_data{
    float y;
};

fluxCallback onInit(fluxGameObject obj, script_data* data){
    data->y = 0;
}

fluxCallback onUpdate(fluxGameObject obj, script_data* data){
    //TraceLog(LOG_INFO,"update2");
    data->y = 0;
}