#ifndef _FLUX_SCENE_H_
#define _FLUX_SCENE_H_

// #include "prefabs.h"
#include "transform.h"

void flux_load_scene(const char* path);

void flux_reset_scene(void);

void flux_close_scene(void);

void flux_draw_scene(void);

#endif