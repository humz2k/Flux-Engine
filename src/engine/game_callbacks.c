/*
                                         _ _ _                _
                                        | | | |              | |
   __ _  __ _ _ __ ___   ___    ___ __ _| | | |__   __ _  ___| | _____
  / _` |/ _` | '_ ` _ \ / _ \  / __/ _` | | | '_ \ / _` |/ __| |/ / __|
 | (_| | (_| | | | | | |  __/ | (_| (_| | | | |_) | (_| | (__|   <\__ \
  \__, |\__,_|_| |_| |_|\___|  \___\__,_|_|_|_.__/ \__,_|\___|_|\_\___/
   __/ |
  |___/
*/

/**
 * @file game_callbacks.c
 * @brief Manages callback functions for specific game events like game loading
 * and closing.
 */

#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#define FLUX_PRIVATE_CALLBACKS
#include "game_callbacks.h"
#include "hqtools/hqtools.h"
#include "raylib.h"

/**
 * @struct GameCallback
 * @brief Holds a list of callback functions to be executed on specific game
 * events.
 *
 * This structure contains an array of function pointers that are called during
 * game events such as load and close.
 */
struct GameCallback {
    void (*callback[FLUX_MAX_GAME_CALLBACKS])(
        void);       /**< Array of callback functions. */
    int n_callbacks; /**< Number of callback functions registered. */
};

/**
 * @brief Initializes a GameCallback structure by setting the number of
 * callbacks to zero.
 * @param callbacks Pointer to the GameCallback structure to initialize.
 */
static void init_game_callback(struct GameCallback* callbacks) {
    callbacks->n_callbacks = 0;
}

/**
 * @brief Adds a callback function to a GameCallback structure.
 * @param callbacks Pointer to the GameCallback structure where the callback
 * should be added.
 * @param new_callback Function pointer to the callback to add.
 */
static void add_callback(struct GameCallback* callbacks,
                         void (*new_callback)(void)) {
    FLUX_ASSERT((callbacks->n_callbacks < FLUX_MAX_GAME_CALLBACKS),
                "FLUX<game_callbacks.c>: reached max callbacks!");
    callbacks->callback[callbacks->n_callbacks] = new_callback;
    callbacks->n_callbacks++;
}

static struct GameCallback
    onGameLoadCallbacks; /**< Callbacks to execute when the game loads. */
static struct GameCallback
    onGameCloseCallbacks; /**< Callbacks to execute when the game closes. */

/**
 * @brief Initializes game event callback structures.
 *
 * Calls `init_game_callback` for both game load and game close events,
 * preparing them to receive callback functions.
 */
void flux_init_game_callbacks(void) {
    TraceLog(LOG_INFO,
             "FLUX<game_callbacks.c>: flux_init_game_callbacks called");
    init_game_callback(&onGameCloseCallbacks);
    init_game_callback(&onGameLoadCallbacks);
}

/**
 * @brief Registers a callback function for a specified game event.
 * @param event Type of game event to register the callback for, specified by an
 * enum FluxGameCallback.
 * @param new_callback Function pointer to the callback to register.
 */
void flux_register_callback(enum FluxGameCallback event,
                            void (*new_callback)(void)) {
    switch (event) {
    case fluxGameCallback_onGameLoad:
        add_callback(&onGameLoadCallbacks, new_callback);
        break;
    case fluxGameCallback_onGameClose:
        add_callback(&onGameCloseCallbacks, new_callback);
        break;
    default:
        TraceLog(LOG_FATAL,
                 "FLUX<game_callbacks.c>: unknown callback registered");
        break;
    }
}

/**
 * @brief Executes all registered callbacks for the game load event.
 */
void flux_game_load(void) {
    TraceLog(LOG_INFO, "FLUX<game_callbacks.c>: flux_game_load called");
    for (int i = 0; i < onGameLoadCallbacks.n_callbacks; i++) {
        onGameLoadCallbacks.callback[i]();
    }
}

/**
 * @brief Executes all registered callbacks for the game close event.
 */
void flux_game_close(void) {
    TraceLog(LOG_INFO, "FLUX<game_callbacks.c>: flux_game_close called");
    for (int i = 0; i < onGameCloseCallbacks.n_callbacks; i++) {
        onGameCloseCallbacks.callback[i]();
    }
}