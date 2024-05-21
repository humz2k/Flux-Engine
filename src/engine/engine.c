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
 * @brief Initializes and manages the main game engine lifecycle, including
 * rendering, updates, and event handling.
 *
 * This file contains functions responsible for initializing the game engine,
 * processing game loops, handling console commands, and cleanly shutting down
 * the game. It integrates components such as the editor, console, and game
 * callbacks.
 */

#include "console.h"
#include "display_size.h"
#include "editor.h"
#include "hqtools/hqtools.h"
#include "loading_screens.h"
#include "pipeline.h"
#include "scene.h"
#include "text_stuff.h"
#include <stdio.h>
#include <stdlib.h>

#define FLUX_PRIVATE_CALLBACKS
#include "game_callbacks.h"

static bool do_quit = false; ///< Flag to control game loop termination.

/**
 * @brief Sets the flag to quit the game.
 *
 * This function is a public interface to set the internal flag indicating that
 * the game should stop running.
 *
 * Once this function is called, the game will quit on the next frame.
 */
void flux_quit_game() { do_quit = true; }

static void console_command_quit(int nargs, const char** args) {
    LOG_FUNC_CALL();
    flux_quit_game();
}

static void set_fps_max_callback(int n_args, const char** args) {
    LOG_FUNC_CALL();
    if (n_args < 2)
        return;
    TraceLog(LOG_INFO, "setting fps_max to %d", atoi(args[1]));
    SetTargetFPS(atoi(args[1]));
}

static void draw_splash_screen(float opacity) {
    float screen_width = GetDisplayWidth();
    float screen_height = GetDisplayHeight();

    Color col = WHITE;
    col.a = Clamp(ceil((opacity) * 255.0f), 0.0f, 250.0f);

    float font_size = 100;

    BeginDrawing();
    ClearBackground(BLACK);
    Vector2 size = MeasureTextEx(editor_font, "Flux Engine", font_size, 1);
    DrawTextEx(
        editor_font, "Flux Engine",
        Vector2Subtract((Vector2){screen_width * 0.5f, screen_height * 0.5f},
                        Vector2Scale(size, 0.5)),
        font_size, 1, col);
    EndDrawing();
}

/**
 * @brief Initializes the game engine.
 *
 * Sets up the window, initializes editor tools, registers console commands, and
 * prepares the rendering pipeline and game callbacks.
 * @param width The width of the game window.
 * @param height The height of the game window.
 * @param name The title of the game window.
 */
void flux_init(int width, int height, const char* name) {

    float splash_screen_init_time = 1.2f;
    float splash_screen_max_time = 3.0f;

    init_editor_tools();

    InitWindow(width, height, name);

    TraceLog(INFO, "running in %s",GetApplicationDirectory());
    #ifdef FLUX_PACKAGE
    ChangeDirectory(GetApplicationDirectory());
    #endif

    load_editor_tools();

    while (GetTime() < splash_screen_init_time) {
        draw_splash_screen(GetTime());
    }

    if (GetTime() > splash_screen_max_time)
        flux_draw_loading_screen("game", 0);

    editor_add_console_command("quit", console_command_quit);
    editor_add_console_command("fps_max", set_fps_max_callback);

    if (GetTime() > splash_screen_max_time)
        flux_draw_loading_screen("game", 0.3);

    flux_init_game_callbacks();

    if (GetTime() > splash_screen_max_time)
        flux_draw_loading_screen("game", 0.5);

    render_init();

    if (GetTime() > splash_screen_max_time)
        flux_draw_loading_screen("game", 0.8);

    flux_reset_scene();

    float cur = GetTime();
    float left = splash_screen_max_time - cur;
    while (GetTime() < splash_screen_max_time) {
        float delta = 1.0f - ((GetTime() - cur) / left);
        draw_splash_screen(delta);
    }

    flux_game_load();
}

/**
 * @brief Closes the game engine.
 *
 * Performs cleanup operations for the scene, game callbacks, rendering
 * pipeline, and window, ensuring a clean shutdown.
 */
void flux_close(void) {
    LOG_FUNC_CALL();
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
static void flux_loop(void) {
    LOG_FUNC_CALL();
    flux_flush_signals();

    flux_scene_script_callback(ONUPDATE);
    flux_scene_script_callback(AFTERUPDATE);

    BeginDrawing();
    ClearBackground(BLACK);

    flux_draw_scene();

    draw_editor_tools();

    DrawFPS(10, 10);

    EndDrawing();
}

/**
 * @brief Runs the main game loop.
 *
 * Continuously processes game loop iterations until a stop condition is met
 * (window close or quit command).
 */
void flux_run(void) {
    LOG_FUNC_CALL();
    while (!WindowShouldClose() && !do_quit) {
        flux_loop();
    }
}