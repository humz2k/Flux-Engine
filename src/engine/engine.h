/**
 * @file engine.h
 **/

#ifndef _FLUX_ENGINE_H_
#define _FLUX_ENGINE_H_

#include "scene.h"

/** @addtogroup engineapi Engine API
 *  @brief The public API for interacting with game states.
 *
 * Initialize Flux with `flux_init()`, close flux with `flux_close()`.
 * Start the main game loop with `flux_run()`.
 * Terminate the game loop with `flux_quit_game()`.
 *
 *  @{
 */

void flux_quit_game();

void flux_init(int width, int height, const char* name);

void flux_close(void);

void flux_run(void);

/** @} */

#endif