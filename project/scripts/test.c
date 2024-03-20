#define SCRIPT test
#include "scripts.h"

script_data{
    int x;
};

void onInit(fluxGameObject obj, script_data* data){
    data->x = 0;
}

void onUpdate(fluxGameObject obj, script_data* data){
    data->x++;
}