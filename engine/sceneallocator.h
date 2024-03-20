#ifndef _FLUX_SCENEALLOCATOR_H_
#define _FLUX_SCENEALLOCATOR_H_

#include <stdlib.h>
#include "raylib.h"

// initializes the scene allocator for the current scene (so should be called every scene load)
void flux_init_scene_allocator(void);

// closes the scene allocator for the current scene (so should be called every scene close)
void flux_close_scene_allocator(void);

// allocates some heap space that will be cleared on scene close
void* flux_scene_alloc(size_t sz);

// loads a model that will be cleared on scene close
Model flux_scene_load_model(const char* path);

#endif