#include "console.h"
#include "editor.h"
#include "hqtools/hqtools.h"
#include "scene.h"
#include "pipeline.h"
#include <stdio.h>
#include <stdlib.h>

#define FLUX_PRIVATE_CALLBACKS
#include "game_callbacks.h"

static bool do_quit = false;

void flux_quit_game() { do_quit = true; }

static void console_command_quit(int nargs, const char** args) {
    flux_quit_game();
}

int run_game() {

    init_editor_tools();

    InitWindow(1000, 800, "test");

    load_editor_tools();

    editor_add_console_command("quit", console_command_quit);

    flux_init_game_callbacks();

    render_init();

    // flux_init_prefabs();
    flux_reset_scene();
    flux_game_load();

    flux_load_scene("/Users/humzaqureshi/GitHub/Flux-Engine/project/scenes/testScene.scene");


    fluxTransform camera_transform;
    camera_transform.pos = (Vector3){0, 1, -3};
    camera_transform.rot = (Vector3){-0.3, 0, 0};
    camera_transform.scale = Vector3One();

    render_set_ka(0.2);

    // flux_scene_instantiate_prefab(fluxPrefab_defaultCamera,
    // camera_transform); flux_scene_instantiate_prefab(fluxPrefab_testPrefab,
    //                              flux_empty_transform());

    while (!WindowShouldClose() && !do_quit) {
        BeginDrawing();

        ClearBackground(BLACK);

        flux_draw_scene();

        draw_editor_tools();

        EndDrawing();
    }

    flux_close_scene();
    flux_game_close();
    render_close();

    CloseWindow();

    close_editor_tools();
    return 0;
}