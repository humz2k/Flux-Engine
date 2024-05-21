#define SCRIPT welcomeNoticeScript
#include "fluxScript.h"
#include "game_callbacks.h"

script_data{

};

fluxCallback onInit(fluxGameObject obj, script_data* data, hstrArray args){

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

}

fluxCallback onDraw2D(fluxGameObject obj, script_data* data){
    //TraceLog(INFO,"test2 recieved signal %d", signal)
    DrawText("Press Enter to begin...",25,100,25,RED);
    DrawText("WASD/Space to move.",25,125,25,RED);
    DrawText("Visit as many websites as possible before being caught by a tracking cookie.",25,150,25,RED);
}