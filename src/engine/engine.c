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
 * @brief Initializes and manages the main game engine lifecycle, including rendering, updates, and event handling.
 *
 * This file contains functions responsible for initializing the game engine, processing game loops,
 * handling console commands, and cleanly shutting down the game. It integrates components such as the
 * editor, console, and game callbacks.
 */

#include "console.h"
#include "editor.h"
#include "hqtools/hqtools.h"
#include "pipeline.h"
#include "scene.h"
#include <stdio.h>
#include <stdlib.h>

#define FLUX_PRIVATE_CALLBACKS
#include "game_callbacks.h"

static bool do_quit = false; ///< Flag to control game loop termination.

/**
 * @brief Sets the flag to quit the game.
 *
 * This function is a public interface to set the internal flag indicating that the game should stop running.
 *
 * Once this function is called, the game will quit on the next frame.
 */
void flux_quit_game() { do_quit = true; }

static void console_command_quit(int nargs, const char** args) {
    flux_quit_game();
}

static void set_fps_max_callback(int n_args, const char** args){
    if (n_args < 2)return;
    TraceLog(LOG_INFO,"setting fps_max to %d",atoi(args[1]));
    SetTargetFPS(atoi(args[1]));
}

/**
 * @brief Initializes the game engine.
 *
 * Sets up the window, initializes editor tools, registers console commands, and prepares the rendering pipeline and game callbacks.
 * @param width The width of the game window.
 * @param height The height of the game window.
 * @param name The title of the game window.
 */
void flux_init(int width, int height, const char* name){
    init_editor_tools();

    InitWindow(width, height, name);

    load_editor_tools();

    editor_add_console_command("quit", console_command_quit);
    editor_add_console_command("fps_max",set_fps_max_callback);

    flux_init_game_callbacks();

    render_init();

    flux_reset_scene();
    flux_game_load();
}

/**
 * @brief Closes the game engine.
 *
 * Performs cleanup operations for the scene, game callbacks, rendering pipeline, and window, ensuring a clean shutdown.
 */
void flux_close(void){
    flux_close_scene();
    flux_game_close();
    render_close();

    CloseWindow();

    close_editor_tools();
}


/**
 * @brief Processes a single frame of the game loop.
 *
 * Handles scene updates, script callbacks, and drawing operations.
 */
static void flux_loop(void){
    flux_flush_signals();

    flux_scene_script_callback(ONUPDATE);
    flux_scene_script_callback(AFTERUPDATE);

    BeginDrawing();

    flux_draw_scene();

    draw_editor_tools();

    DrawFPS(10,10);

    EndDrawing();
}

/**
 * @brief Runs the main game loop.
 *
 * Continuously processes game loop iterations until a stop condition is met (window close or quit command).
 */
void flux_run(void){
    while (!WindowShouldClose() && !do_quit) {
        flux_loop();
    }
}