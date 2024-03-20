#ifndef _FLUX_SCENEALLOCATOR_H_
#define _FLUX_SCENEALLOCATOR_H_

#include <stdlib.h>
#include "raylib.h"

// initializes the scene allocator for the current scene (so should be called every scene load)
void fluxInitSceneAllocator(void);

// closes the scene allocator for the current scene (so should be called every scene close)
void fluxCloseSceneAllocator(void);

// allocates some heap space that will be cleared on scene close
void* fluxSceneAlloc(size_t sz);

// loads a model that will be cleared on scene close
Model fluxSceneLoadModel(const char* path);

#endif