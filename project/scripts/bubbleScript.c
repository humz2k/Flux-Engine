#define SCRIPT bubbleScript
#include "fluxScript.h"
#include "game_callbacks.h"

extern bool mailboxes[1000];
extern bool bubble[1000];
extern fluxTransform player_transform;
extern int n_remaining;
extern int n_remaining2;

script_data{
    int id;
};

fluxCallback onInit(fluxGameObject obj, script_data* data, hstrArray args){
    data->id = n_remaining2;
    n_remaining2++;
}

fluxCallback onUpdate(fluxGameObject obj, script_data* data){
    flux_gameobject_set_visible(obj,bubble[data->id]);
}

fluxCallback afterUpdate(fluxGameObject obj, script_data* data){

}

fluxCallback onDestroy(fluxGameObject obj, script_data* data){

}