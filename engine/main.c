#include <stdlib.h>
#include <stdio.h>
#include "scene.h"

#define FLUX_PRIVATE_CALLBACKS
#include "game_callbacks.h"

int main(){
    InitWindow(400,400,"test");

    flux_init_game_callbacks();
    flux_init_prefabs();
    flux_reset_scene();


    fluxTransform camera_transform;
    camera_transform.pos = (Vector3){0,1,-3};
    camera_transform.rot = (Vector3){-0.3,0,0};
    camera_transform.scale = Vector3One();

    flux_scene_instantiate_prefab(fluxPrefab_defaultCamera, camera_transform);
    flux_scene_instantiate_prefab(fluxPrefab_testPrefab, flux_empty_transform());

    while (!WindowShouldClose()){
        BeginDrawing();

        ClearBackground(BLACK);

        flux_draw_scene();

        EndDrawing();
    }

    flux_close_scene();
    flux_game_load();
    flux_game_close();

    CloseWindow();
    return 0;
}