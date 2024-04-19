#ifndef _FLUX_GAME_CALLBACKS_H_
#define _FLUX_GAME_CALLBACKS_H_

enum FluxGameCallback {
    fluxGameCallback_onGameLoad,
    fluxGameCallback_onGameClose
};

void flux_register_callback(enum FluxGameCallback event,
                            void (*new_callback)(void));

#ifdef FLUX_PRIVATE_CALLBACKS

void flux_init_game_callbacks(void);

void flux_game_load(void);

void flux_game_close(void);

#endif
#endif