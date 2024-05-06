#define SCRIPT lightmanager
#include "fluxScript.h"

script_data{
    float ka;
};

fluxCallback onInit(fluxGameObject obj, script_data* data, hstrArray args){
    data->ka = 0.2;
    //render_set_ka(0.2);
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

    for (int i = 0; i < hstr_array_len(args); i++){
        hstr arg = hstr_incref(hstr_array_get(args,i));
        hstrArray split = hstr_split(arg,":");
        if (hstr_array_len(split) == 2){
            const char* argument = hstr_unpack(hstr_array_get(split,0));
            const char* value = hstr_unpack(hstr_array_get(split,1));
            if (strcmp(argument,"ka") == 0){
                data->ka = atof(value);
                TraceLog(LOG_INFO,"ka = %g",data->ka);
            }
        }
        hstr_array_delete(split);
        hstr_decref(arg);
    }

    render_set_ka(data->ka);
}

fluxCallback onUpdate(fluxGameObject obj, script_data* data){

}