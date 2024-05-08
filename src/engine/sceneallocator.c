/**
 * @file sceneallocator.c
 **/

#include "sceneallocator.h"
#include "config.h"
#include "gameobject.h"
#include "hqtools/hqtools.h"
#include "raylib.h"
#include "raymath.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// stores all the allocations of the current scene in resizable array
static void** allocations = NULL;
// number of active allocations
static int n_allocations = 0;
// size of the allocations array (n_allocations may be less than
// allocations_size)
static int allocations_size = 0;

// initializes the scene allocator for the current scene (so should be called
// every scene load)
void flux_init_scene_allocator(void) {
    TraceLog(LOG_INFO, "FLUX<sceneallocator.c>: initializing sceneallocator");
    // allocations and models must be NULL
    // if not, then something bad happened
    FLUX_ASSERT((NULL == allocations),
                "FLUX<sceneallocator.c>: was fluxCloseSceneAllocator called on "
                "scene close?");
    // set number of allocations and models to 0
    n_allocations = 0;
    // and the initial size of the allocations and models arrays to be 10
    allocations_size = 10;
    // malloc accordingly
    assert(allocations = (void**)malloc(sizeof(void*) * allocations_size));
    TraceLog(LOG_INFO, "FLUX<sceneallocator.c>: initialized sceneallocator");
}

// closes the scene allocator for the current scene (so should be called every
// scene close)
void flux_close_scene_allocator(void) {
    TraceLog(LOG_INFO, "FLUX<sceneallocator.c>: closing sceneallocator");
    FLUX_ASSERT((n_allocations >= 0),
                "FLUX<sceneallocator.c>: n_allocations was less than 0???");
    FLUX_ASSERT(
        (n_allocations < allocations_size),
        "FLUX<sceneallocator.c>: n_allocations is less than allocations_size!");

    // loop through active allocations and free the memory
    for (int i = 0; i < n_allocations; i++) {
        free(allocations[i]);
    }
    // finally free the allocations array and set to NULL
    free(allocations);
    allocations = NULL;

    TraceLog(LOG_INFO,
             "FLUX<sceneallocator.c>: closed sceneallocator (%d allocations)",
             n_allocations);
}

// allocates some heap space that will be cleared on scene close
void* flux_scene_alloc(size_t sz) {
    TraceLog(LOG_INFO,
             "FLUX<sceneallocator.c>: sceneallocator allocating %lu bytes", sz);
    FLUX_ASSERT((n_allocations >= 0),
                "FLUX<sceneallocator.c>: n_allocations was less than 0???");
    // if we are out of space, we must grow the array
    if (n_allocations >= allocations_size) {
        TraceLog(LOG_INFO,
                 "FLUX<sceneallocator.c>: sceneallocator growing allocations "
                 "buffer (old size = %d, new size = %d)",
                 allocations_size, allocations_size * 2);
        FLUX_ASSERT((allocations_size > 0),
                    "FLUX<sceneallocator.c>: allocations_size should never be "
                    "less than 1!");
        // double the size of the allocations array
        allocations_size *= 2;
        // realloc accordingly
        assert(allocations =
                   realloc(allocations, sizeof(void*) * allocations_size));
        FLUX_ASSERT((n_allocations < allocations_size),
                    "FLUX<sceneallocator.c>: n_allocations was still bigger "
                    "than allocations_size after resize!");
    }
    // get the pointer to be returned
    void* out;
    assert(out = (void*)malloc(sz));
    // store this pointer in the correct place in allocations
    allocations[n_allocations] = out;
    // and then increment n_allocations
    n_allocations++;
    // finally return the malloced data
    return out;
}