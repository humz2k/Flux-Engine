#define SCRIPT test2
#include "scripts.h"

script_data{
    float y;
};

void onInit(fluxGameObject obj, script_data* data){
    data->y = 0;
}