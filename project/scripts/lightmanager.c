#define SCRIPT lightmanager
#include "fluxScript.h"

script_data{

};

fluxCallback onInit(fluxGameObject obj, script_data* data){
    render_set_ka(0.2);
    render_light_enable(0);
    render_light_set_type(0,0);
    render_light_set_cL(0,WHITE);
    render_light_set_kd(0,0.7);
    render_light_set_ks(0,0.3);
    render_light_set_L(0,Vector3One());
    render_light_set_p(0,200);

    render_light_enable(1);
    render_light_set_type(1,0);
    render_light_set_cL(1,WHITE);
    render_light_set_kd(1,0.7);
    render_light_set_ks(1,0.3);
    render_light_set_L(1,(Vector3){0,2,-1});
    render_light_set_p(1,200);
}

fluxCallback onUpdate(fluxGameObject obj, script_data* data){

}