/**
 * @file config.h
 **/

#ifndef _FLUX_CONFIG_H_
#define _FLUX_CONFIG_H_

#define FLUX_MAX_SCRIPTS 50
#define FLUX_MAX_CHILDREN 50
#define FLUX_MAX_GAME_CALLBACKS 50
#define FLUX_MAX_GAMEOBJECTS 1000
#define FLUX_MAX_PENDING_SIGNALS 100

#include "raylib.h"

#define FLUX_ASSERT(cond, ...)                                                 \
    if (!(cond))                                                               \
    TraceLog(LOG_FATAL, __VA_ARGS__)

#endif