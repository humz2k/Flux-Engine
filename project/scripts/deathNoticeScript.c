#define SCRIPT deathNoticeScript
#include "fluxScript.h"
#include "game_callbacks.h"
extern hstr killed_by;

script_data{

};

fluxCallback onInit(fluxGameObject obj, script_data* data, hstrArray args){
    hstr_incref(killed_by);
}

fluxCallback onUpdate(fluxGameObject obj, script_data* data){
    //TraceLog(LOG_INFO,"update2");
    if (IsKeyPressed(KEY_ENTER)){
        flux_send_signal(5);
    }

}

fluxCallback onSignal(fluxGameObject obj, script_data* data, int signal){
    //TraceLog(INFO,"test2 recieved signal %d", signal);
}

fluxCallback onDraw(fluxGameObject obj, script_data* data){
    //TraceLog(INFO,"test2 recieved signal %d", signal);
}

fluxCallback onDestroy(fluxGameObject obj, script_data* data){
    hstr_decref(killed_by);
}

fluxCallback onDraw2D(fluxGameObject obj, script_data* data){
    //TraceLog(INFO,"test2 recieved signal %d", signal)
    char text[1000];
    sprintf(text,"You were killed by %s. Press Enter to try again.",hstr_unpack(killed_by));
    DrawText(text,100,100,25,RED);
}