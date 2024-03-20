#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "raylib.h"
#include "raymath.h"
#include "gameobject.h"
#include "sceneallocator.h"

// stores all the allocations of the current scene in resizable array
static void** allocations = NULL;
// number of active allocations
static int n_allocations = 0;
// size of the allocations array (n_allocations may be less than allocations_size)
static int allocations_size = 0;

// similarly for models
static Model* models = NULL;
static int n_models = 0;
static int models_size = 0;


// initializes the scene allocator for the current scene (so should be called every scene load)
void flux_init_scene_allocator(void){
    TraceLog(LOG_INFO,"initializing sceneallocator");
    // allocations and models must be NULL
    // if not, then something bad happened
    assert((NULL == allocations) && (NULL == models) && "was fluxCloseSceneAllocator called on scene close?");
    // set number of allocations and models to 0
    n_allocations = 0;
    n_models = 0;
    // and the initial size of the allocations and models arrays to be 10
    allocations_size = 10;
    models_size = 10;
    // malloc accordingly
    assert(allocations = (void**)malloc(sizeof(void*) * allocations_size));
    assert(models = (Model*)malloc(sizeof(Model) * models_size));
    TraceLog(LOG_INFO,"initialized sceneallocator");
}

// closes the scene allocator for the current scene (so should be called every scene close)
void flux_close_scene_allocator(void){
    TraceLog(LOG_INFO,"closing sceneallocator");
    assert((n_allocations >= 0) && "n_allocations was less than 0???");
    assert((n_allocations < allocations_size) && "n_allocations is less than allocations_size!");
    assert((n_models >= 0) && "n_models was less than 0???");
    assert((n_models < models_size) && "n_models is less than models_size!");
    // loop through active allocations and free the memory
    for (int i = 0; i < n_allocations; i++){
        free(allocations[i]);
    }
    // finally free the allocations array and set to NULL
    free(allocations); allocations = NULL;
    // loop through active models and free the Model
    for (int i = 0; i < n_models; i++){
        UnloadModel(models[i]);
    }
    free(models); models = NULL;
    TraceLog(LOG_INFO,"closed sceneallocator (%d allocations, %d models)",n_allocations,n_models);
}

// allocates some heap space that will be cleared on scene close
void* flux_scene_alloc(size_t sz){
    TraceLog(LOG_INFO,"sceneallocator allocating %lu bytes",sz);
    assert((n_allocations >= 0) && "n_allocations was less than 0???");
    // if we are out of space, we must grow the array
    if (n_allocations >= allocations_size){
        TraceLog(LOG_INFO,"sceneallocator growing allocations buffer (old size = %d, new size = %d)",allocations_size,allocations_size*2);
        assert((allocations_size > 0) && "allocations_size should never be less than 1!");
        // double the size of the allocations array
        allocations_size *= 2;
        // realloc accordingly
        assert(allocations = realloc(allocations,sizeof(void*) * allocations_size));
        assert((n_allocations < allocations_size) && "n_allocations was still bigger than allocations_size after resize!");
    }
    // get the pointer to be returned
    void* out; assert(out = (void*)malloc(sz));
    // store this pointer in the correct place in allocations
    allocations[n_allocations] = out;
    // and then increment n_allocations
    n_allocations++;
    // finally return the malloced data
    return out;
}

Model flux_scene_load_model(const char* path){
    assert((n_models >= 0) && "n_models was less than 0???");
    // if we are out of space, we must grow the array
    if (n_models >= models_size){
        assert((models_size > 0) && "models_size should never be less than 1!");
        // double the size of the models array
        models_size *= 2;
        // realloc accordingly
        assert(models = (Model*)realloc(models,sizeof(Model) * models_size));
        assert((n_models < models_size) && "n_models was still bigger than models_size after resize!");
    }
    // get the model to be returned
    Model out = LoadModel(path);
    // store this model in the correct place in models
    models[n_models] = out;
    // and then increment n_models
    n_models++;
    // finally return the loaded model
    return out;
}