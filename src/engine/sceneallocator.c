/**
 * @file sceneallocator.c
 * @brief Manages dynamic memory allocation for game scene resources, ensuring all allocations are tracked and freed appropriately.
 *
 * Provides functions to initialize and close the memory allocator for scenes, and to handle memory allocations that are automatically tracked
 * and freed when the scene is closed. This helps prevent memory leaks by tying memory lifetimes to scene lifetimes.
 */

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

static void** allocations = NULL; ///< Pointer to dynamically allocated memory blocks for the scene.
static int n_allocations = 0; ///< Number of active allocations.
static int allocations_size = 0; ///< Current size of the allocations array, may be larger than n_allocations.

/**
 * @brief Initializes the scene allocator for managing memory allocations within a scene.
 *
 * This function prepares the allocator for use, ensuring it starts in a clean state with no existing allocations.
 * It should be called every time a scene is loaded to reset the allocator's state.
 */
void flux_init_scene_allocator(void) {
    LOG_FUNC_CALL();
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

/**
 * @brief Closes the scene allocator, freeing all tracked memory allocations.
 *
 * This function should be called every time a scene is closed to ensure that all memory allocated for the scene
 * is properly freed and that the allocator is reset for future use.
 */
void flux_close_scene_allocator(void) {
    LOG_FUNC_CALL();
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

/**
 * @brief Allocates memory that will be automatically managed and freed when the scene closes.
 *
 * This function allocates memory and automatically tracks it to ensure it is freed when the scene is closed,
 * helping to manage memory efficiently and prevent leaks within scene lifecycles.
 * @param sz Size of the memory block to allocate.
 * @return Pointer to the allocated memory block.
 */
void* flux_scene_alloc(size_t sz) {
    LOG_FUNC_CALL();
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