#ifndef _FLUX_SCENE_H_
#define _FLUX_SCENE_H_

#include "transform.h"
#include "prefabs.h"

void flux_reset_scene(void);

void flux_close_scene(void);

void flux_scene_instantiate_prefab(enum fluxPrefabID prefab, fluxTransform transform);

void flux_draw_scene(void);

#endif