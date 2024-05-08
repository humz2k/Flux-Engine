/**
 * @file engine.h
 **/

#ifndef _FLUX_ENGINE_H_
#define _FLUX_ENGINE_H_

#include "scene.h"

void flux_quit_game();

void flux_init(int width, int height, const char* name);

void flux_close(void);

void flux_run(void);

#endif