#include "engine.h"
#include "game_callbacks.h"

void signal_handler(int signal){
    TraceLog(LOG_INFO,"received signal %d",signal);
}

int run(){
    flux_init(1000,800,"test");

    flux_register_signal_callback(signal_handler);

    flux_load_scene("/Users/humzaqureshi/GitHub/Flux-Engine/project/scenes/"
                    "testScene.scene");

    flux_send_signal(0);

    flux_run();

    flux_close();
    return 0;
}