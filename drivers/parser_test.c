#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "hqtools/hqtools.h"
#include "prefab.h"

int main(){

    //InitWindow(200,200,"test");

    hq_allocator_init_global();

    fluxParsedPrefab testPrefab = parser_read_prefab("/Users/humzaqureshi/GitHub/Flux-Engine/project/prefabs/testPrefab.prefab");
    fluxParsedPrefab defaultCamera = parser_read_prefab("/Users/humzaqureshi/GitHub/Flux-Engine/project/prefabs/defaultCamera.prefab");

    parser_delete_parsed_prefab(testPrefab);
    parser_delete_parsed_prefab(defaultCamera);

    hq_allocator_delete_global();

    //CloseWindow();
    return 0;
}