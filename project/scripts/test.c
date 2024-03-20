#define SCRIPT test
#include "fluxScript.h"

script_data{
    int x;
};

fluxCallback onInit(fluxGameObject obj, script_data* data){
    data->x = 0;
}

fluxCallback onUpdate(fluxGameObject obj, script_data* data){
    data->x++;
}