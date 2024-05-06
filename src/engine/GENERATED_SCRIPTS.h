
struct test_fluxData;
struct lightmanager_fluxData;
struct test2_fluxData;
#define FLUX_GAMEOBJECT_TYPE_ONLY
#include "gameobject.h"
#include "sceneallocator.h"
#ifdef FLUX_SCRIPTS_IMPLEMENTATION
#define SCRIPT test
#include "fluxScript.h"

script_data{
    int x;
};

fluxCallback onInit(fluxGameObject obj, script_data* data){
    data->x = 0;
    /*render_set_ka(0.2);
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
    render_light_set_p(1,200);*/
}

fluxCallback onUpdate(fluxGameObject obj, script_data* data){
    //TraceLog(LOG_INFO,"UPDATE...");
    data->x++;
}

fluxCallback afterUpdate(fluxGameObject obj, script_data* data){}
fluxCallback onDestroy(fluxGameObject obj, script_data* data){}
fluxCallback onDraw(fluxGameObject obj, script_data* data){}
fluxCallback onDraw2D(fluxGameObject obj, script_data* data){}

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

fluxCallback afterUpdate(fluxGameObject obj, script_data* data){}
fluxCallback onDestroy(fluxGameObject obj, script_data* data){}
fluxCallback onDraw(fluxGameObject obj, script_data* data){}
fluxCallback onDraw2D(fluxGameObject obj, script_data* data){}

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

fluxCallback afterUpdate(fluxGameObject obj, script_data* data){}
fluxCallback onDestroy(fluxGameObject obj, script_data* data){}
fluxCallback onDraw(fluxGameObject obj, script_data* data){}
fluxCallback onDraw2D(fluxGameObject obj, script_data* data){}


#endif

enum fluxScriptID{fluxEmptyScript,fluxScript_test,fluxScript_lightmanager,fluxScript_test2};


struct fluxScriptStruct;
typedef struct fluxScriptStruct* fluxScript;
#ifdef FLUX_SCRIPTS_IMPLEMENTATION
struct fluxScriptStruct{
    enum fluxScriptID id;
    union {
        void* raw;
        struct test_fluxData* test_fluxData;
        struct lightmanager_fluxData* lightmanager_fluxData;
        struct test2_fluxData* test2_fluxData;
    };
};
#endif



void fluxCallback_onUpdate(fluxGameObject obj, fluxScript script)
#ifdef FLUX_SCRIPTS_IMPLEMENTATION
{
    switch(script->id){
        
        case fluxScript_test:
            test_fluxCallback_onUpdate(obj,script->test_fluxData);
            break;


        case fluxScript_lightmanager:
            lightmanager_fluxCallback_onUpdate(obj,script->lightmanager_fluxData);
            break;


        case fluxScript_test2:
            test2_fluxCallback_onUpdate(obj,script->test2_fluxData);
            break;

        default:
            //assert((1 == 0) && "something terrible happened at compile time!");
            break;
    }
}
#else
;
#endif




void fluxCallback_afterUpdate(fluxGameObject obj, fluxScript script)
#ifdef FLUX_SCRIPTS_IMPLEMENTATION
{
    switch(script->id){
        
        case fluxScript_test:
            test_fluxCallback_afterUpdate(obj,script->test_fluxData);
            break;


        case fluxScript_lightmanager:
            lightmanager_fluxCallback_afterUpdate(obj,script->lightmanager_fluxData);
            break;


        case fluxScript_test2:
            test2_fluxCallback_afterUpdate(obj,script->test2_fluxData);
            break;

        default:
            //assert((1 == 0) && "something terrible happened at compile time!");
            break;
    }
}
#else
;
#endif




void fluxCallback_onInit(fluxGameObject obj, fluxScript script)
#ifdef FLUX_SCRIPTS_IMPLEMENTATION
{
    switch(script->id){
        
        case fluxScript_test:
            test_fluxCallback_onInit(obj,script->test_fluxData);
            break;


        case fluxScript_lightmanager:
            lightmanager_fluxCallback_onInit(obj,script->lightmanager_fluxData);
            break;


        case fluxScript_test2:
            test2_fluxCallback_onInit(obj,script->test2_fluxData);
            break;

        default:
            //assert((1 == 0) && "something terrible happened at compile time!");
            break;
    }
}
#else
;
#endif




void fluxCallback_onDestroy(fluxGameObject obj, fluxScript script)
#ifdef FLUX_SCRIPTS_IMPLEMENTATION
{
    switch(script->id){
        
        case fluxScript_test:
            test_fluxCallback_onDestroy(obj,script->test_fluxData);
            break;


        case fluxScript_lightmanager:
            lightmanager_fluxCallback_onDestroy(obj,script->lightmanager_fluxData);
            break;


        case fluxScript_test2:
            test2_fluxCallback_onDestroy(obj,script->test2_fluxData);
            break;

        default:
            //assert((1 == 0) && "something terrible happened at compile time!");
            break;
    }
}
#else
;
#endif




void fluxCallback_onDraw(fluxGameObject obj, fluxScript script)
#ifdef FLUX_SCRIPTS_IMPLEMENTATION
{
    switch(script->id){
        
        case fluxScript_test:
            test_fluxCallback_onDraw(obj,script->test_fluxData);
            break;


        case fluxScript_lightmanager:
            lightmanager_fluxCallback_onDraw(obj,script->lightmanager_fluxData);
            break;


        case fluxScript_test2:
            test2_fluxCallback_onDraw(obj,script->test2_fluxData);
            break;

        default:
            //assert((1 == 0) && "something terrible happened at compile time!");
            break;
    }
}
#else
;
#endif




void fluxCallback_onDraw2D(fluxGameObject obj, fluxScript script)
#ifdef FLUX_SCRIPTS_IMPLEMENTATION
{
    switch(script->id){
        
        case fluxScript_test:
            test_fluxCallback_onDraw2D(obj,script->test_fluxData);
            break;


        case fluxScript_lightmanager:
            lightmanager_fluxCallback_onDraw2D(obj,script->lightmanager_fluxData);
            break;


        case fluxScript_test2:
            test2_fluxCallback_onDraw2D(obj,script->test2_fluxData);
            break;

        default:
            //assert((1 == 0) && "something terrible happened at compile time!");
            break;
    }
}
#else
;
#endif



fluxScript flux_allocate_script(enum fluxScriptID id)
#ifdef FLUX_SCRIPTS_IMPLEMENTATION
{
    fluxScript out = (fluxScript)flux_scene_alloc(sizeof(struct fluxScriptStruct));
    out->id = id;
    size_t sz = 0;
    switch(id){
        case fluxScript_test:
            sz = sizeof(struct test_fluxData);
            break;
        case fluxScript_lightmanager:
            sz = sizeof(struct lightmanager_fluxData);
            break;
        case fluxScript_test2:
            sz = sizeof(struct test2_fluxData);
            break;
        default:
            //assert((1 == 0) && "something terrible happened at build time!");
            break;
    }
    out->raw = flux_scene_alloc(sz);
    return out;
}
#else
;
#endif


#ifdef FLUX_SCRIPTS_IMPLEMENTATION
static const char* SCRIPT_NAME_TO_ENUM[4]={[fluxEmptyScript] = "empty_script",[fluxScript_test] = "test",[fluxScript_lightmanager] = "lightmanager",[fluxScript_test2] = "test2"};
#endif
