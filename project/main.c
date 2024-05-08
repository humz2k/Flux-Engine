#include "engine.h"

int main(){
    flux_init(1000,800,"test");

    flux_load_scene("/Users/humzaqureshi/GitHub/Flux-Engine/project/scenes/"
                    "testScene.scene");

    flux_run();

    flux_close();
}