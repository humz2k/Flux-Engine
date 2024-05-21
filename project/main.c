#include "engine.h"
#include "game_callbacks.h"
#include "console.h"

float move_speed = 5.0f;
float sensitivity = 0.001f;
int n_remaining = 0;
int n_remaining2 = 0;

bool mailboxes[1000] = {0};
bool bubble[1000];

fluxTransform player_transform;

void signal_handler(int signal){
    TraceLog(LOG_INFO,"received signal %d",signal);
    if (signal == 10){
        flux_quit_game();
    }
}

static void set_move_speed_callback(int n_args, const char** args){
    if (n_args < 2)return;
    move_speed = atof(args[1]);
}

static void set_sensitivity_callback(int n_args, const char** args){
    if (n_args < 2)return;
    sensitivity = atof(args[1]);
}

int run(){

    flux_init(1000,800,"test");

    for (int i = 0; i < 1000; i++){
        bubble[i] = true;
    }

    editor_add_console_command("set_move_speed",set_move_speed_callback);
    editor_add_console_command("set_sensitivity",set_sensitivity_callback);

    flux_register_signal_callback(signal_handler);

    flux_load_scene("/Users/humzaqureshi/GitHub/Flux-Engine/project/scenes/"
                    "testScene.scene");

    flux_send_signal(0);

    flux_run();

    flux_close();
    return 0;
}