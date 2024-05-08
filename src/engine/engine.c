/*
                   _
                  (_)
   ___ _ __   __ _ _ _ __   ___
  / _ \ '_ \ / _` | | '_ \ / _ \
 |  __/ | | | (_| | | | | |  __/
  \___|_| |_|\__, |_|_| |_|\___|
              __/ |
             |___/
*/

/**
 * @file engine.c
 **/

#include "console.h"
#include "editor.h"
#include "hqtools/hqtools.h"
#include "pipeline.h"
#include "scene.h"
#include <stdio.h>
#include <stdlib.h>

#define FLUX_PRIVATE_CALLBACKS
#include "game_callbacks.h"

static bool do_quit = false;

void flux_quit_game() { do_quit = true; }

static void console_command_quit(int nargs, const char** args) {
    flux_quit_game();
}

void flux_init(int width, int height, const char* name){
    init_editor_tools();

    InitWindow(width, height, name);

    load_editor_tools();

    editor_add_console_command("quit", console_command_quit);

    flux_init_game_callbacks();

    render_init();

    flux_reset_scene();
    flux_game_load();
}

void flux_close(void){
    flux_close_scene();
    flux_game_close();
    render_close();

    CloseWindow();

    close_editor_tools();
}

static void flux_loop(void){
    flux_scene_script_callback(ONUPDATE);
    flux_scene_script_callback(AFTERUPDATE);

    BeginDrawing();

    flux_draw_scene();

    draw_editor_tools();

    EndDrawing();
}

void flux_run(void){
    while (!WindowShouldClose() && !do_quit) {
        flux_loop();
    }
}