#include "console.h"
#include "engine.h"
#include "game_callbacks.h"
#include "pipeline.h"

void signal_handler(int signal) {
    TraceLog(LOG_INFO, "received signal %d", signal);
}

int run() {
    flux_init(1024, 1024, "test");
    SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
    ToggleFullscreen();

    flux_register_signal_callback(signal_handler);

    SetTargetFPS(200);

    flux_load_scene("project/scenes/emptyScene.scene");

    flux_send_signal(0);

    flux_run();

    flux_close();
    return 0;
}