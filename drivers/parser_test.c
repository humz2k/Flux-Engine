#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "hqtools/hqtools.h"
#include "prefab_parser.h"
#include "scene_parser.h"
#include "prefabs.h"

int main(){

    InitWindow(200,200,"test");

    hq_allocator_init_global();

    //fluxParsedScene testScene = parser_read_scene("/Users/humzaqureshi/GitHub/Flux-Engine/project/scenes/testScene.scene");

    //parser_delete_parsed_scene(testScene);
    fluxParsedPrefab parsed_prefab = parser_read_prefab("/Users/humzaqureshi/GitHub/Flux-Engine/project/prefabs/testPrefab.prefab");

    fluxPrefab prefab = flux_load_prefab(parsed_prefab);

    flux_delete_prefab(prefab);

    parser_delete_parsed_prefab(parsed_prefab);

    hq_allocator_delete_global();

    CloseWindow();
    return 0;
}