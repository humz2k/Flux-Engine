/**
 * @file game_callbacks.h
 **/

#ifndef _FLUX_GAME_CALLBACKS_H_
#define _FLUX_GAME_CALLBACKS_H_

enum FluxGameCallback {
    fluxGameCallback_onGameLoad,
    fluxGameCallback_onGameClose
};

void flux_register_callback(enum FluxGameCallback event,
                            void (*new_callback)(void));

void flux_register_signal_callback(void(*signal_callback)(int));

void flux_send_signal(int signal);

#ifdef FLUX_PRIVATE_CALLBACKS

void flux_init_game_callbacks(void);

void flux_game_load(void);

void flux_game_close(void);

void flux_flush_signals(void);

#endif
#endif