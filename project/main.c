#include "console.h"
#include "engine.h"
#include "game_callbacks.h"

float move_speed = 5.0f;
float sensitivity = 0.001f;
int n_remaining = 0;
int n_remaining2 = 0;
int n_total = 0;

bool mailboxes[1000] = {0};
bool bubble[1000];

hstr killed_by;

fluxTransform player_transform;

void signal_handler(int signal) {
    TraceLog(LOG_INFO, "received signal %d", signal);
    if (signal == 10) {
        TraceLog(LOG_INFO, "killed by %s", hstr_unpack(killed_by));

        flux_close_scene();
        flux_reset_scene();
        flux_load_scene("project/scenes/emptyScene.scene");

        hstr_decref(killed_by);
    }
    if (signal == 5) {
        flux_close_scene();
        flux_reset_scene();
        n_remaining2 = 0;
        n_remaining = 0;
        for (int i = 0; i < 1000; i++) {
            bubble[i] = true;
        }
        flux_load_scene("project/scenes/testScene.scene");
    }
}

static void set_move_speed_callback(int n_args, const char** args) {
    if (n_args < 2)
        return;
    move_speed = atof(args[1]);
}

static void set_sensitivity_callback(int n_args, const char** args) {
    if (n_args < 2)
        return;
    sensitivity = atof(args[1]);
}

int run() {

    flux_init(1000, 800, "test");

    for (int i = 0; i < 1000; i++) {
        bubble[i] = true;
    }

    editor_add_console_command("set_move_speed", set_move_speed_callback);
    editor_add_console_command("set_sensitivity", set_sensitivity_callback);

    flux_register_signal_callback(signal_handler);

    SetTargetFPS(200);

    flux_load_scene("project/scenes/welcomeScene.scene");

    flux_send_signal(0);

    flux_run();

    flux_close();
    return 0;
}