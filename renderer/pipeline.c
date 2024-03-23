#include "raylib.h"
#include "raymath.h"

static Camera3D current_camera;

void render_begin(Camera3D camera){
    current_camera = camera;
    BeginMode3D(camera);
}

void render_end(void){
    EndMode3D();
}

