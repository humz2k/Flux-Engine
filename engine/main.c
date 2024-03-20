#include <stdlib.h>
#include <stdio.h>
#include "sceneallocator.h"

#define FLUX_PRIVATE_CALLBACKS
#include "game_callbacks.h"

#include "prefabs.h"

int main(){
    flux_init_game_callbacks();
    flux_init_prefabs();

    flux_game_load();

    flux_game_close();
    return 0;
}