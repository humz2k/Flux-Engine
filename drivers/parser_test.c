#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "hqtools/hqtools.h"
#include "prefab.h"

int main(){

    //InitWindow(200,200,"test");

    hq_allocator_init_global();

    fluxParsedPrefab prefab = parser_read_prefab("/Users/humzaqureshi/GitHub/Flux-Engine/project/prefabs/testPrefab.prefab");

    parser_delete_parsed_prefab(prefab);

    hq_allocator_delete_global();

    //CloseWindow();
    return 0;
}