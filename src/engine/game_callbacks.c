#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#define FLUX_PRIVATE_CALLBACKS
#include "game_callbacks.h"
#include "hqtools/hqtools.h"
#include "raylib.h"

// holds functions to be run on some event
struct GameCallback {
    // functions to be run
    void (*callback[FLUX_MAX_GAME_CALLBACKS])(void);
    // number of functions registered
    int n_callbacks;
};

// initialze the number of callbacks to 0
static void init_game_callback(struct GameCallback* callbacks) {
    callbacks->n_callbacks = 0;
}

// add a callback to a struct GameCallback
static void add_callback(struct GameCallback* callbacks,
                         void (*new_callback)(void)) {
    FLUX_ASSERT((callbacks->n_callbacks < FLUX_MAX_GAME_CALLBACKS),
                "FLUX<game_callbacks.c>: reached max callbacks!");
    callbacks->callback[callbacks->n_callbacks] = new_callback;
    callbacks->n_callbacks++;
}

static struct GameCallback onGameLoadCallbacks;
static struct GameCallback onGameCloseCallbacks;

void flux_init_game_callbacks(void) {
    TraceLog(LOG_INFO,
             "FLUX<game_callbacks.c>: flux_init_game_callbacks called");
    init_game_callback(&onGameCloseCallbacks);
    init_game_callback(&onGameLoadCallbacks);
}

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

void flux_game_load(void) {
    TraceLog(LOG_INFO, "FLUX<game_callbacks.c>: flux_game_load called");
    for (int i = 0; i < onGameLoadCallbacks.n_callbacks; i++) {
        onGameLoadCallbacks.callback[i]();
    }
}

void flux_game_close(void) {
    TraceLog(LOG_INFO, "FLUX<game_callbacks.c>: flux_game_close called");
    for (int i = 0; i < onGameCloseCallbacks.n_callbacks; i++) {
        onGameCloseCallbacks.callback[i]();
    }
}