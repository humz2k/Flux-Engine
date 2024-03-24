#include <stdlib.h>
#include <stdio.h>
#include "pipeline.h"
#include "editor.h"
#include "console.h"
#include "transform.h"

static bool do_quit = false;

static void quit_callback(int n_args, const char** args){
    do_quit = true;
}

int main(){

    init_editor_tools();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200,800,"flux_render_test");

    load_editor_tools();

    editor_add_console_command("quit",quit_callback);

    SetTargetFPS(200);

    render_init();

    Camera3D cam;
    cam.fovy = 45;
    cam.position = (Vector3){0,2,-10};
    cam.projection = CAMERA_PERSPECTIVE;
    cam.up = (Vector3){0,1,0};
    cam.target = Vector3Zero();

    Model sphere = LoadModel("/Users/humzaqureshi/GitHub/Flux-Engine/drivers/assets/sphere.obj");
    fluxTransform sphere_tranform = flux_empty_transform();
    sphere_tranform.pos.y = 1;

    Model plane = LoadModelFromMesh(GenMeshPlane(10,10,10,10));
    fluxTransform plane_transform = flux_empty_transform();

    render_set_ka(0.2);
    render_light_enable(0);
    render_light_set_type(0,0);
    render_light_set_cL(0,WHITE);
    render_light_set_kd(0,0.7);
    render_light_set_ks(0,0.3);
    render_light_set_L(0,Vector3One());
    render_light_set_p(0,200);

    while (!WindowShouldClose() && !do_quit){
        BeginDrawing();

        ClearBackground(BLACK);

        UpdateCamera(&cam,CAMERA_ORBITAL);

        render_begin(cam);

        render_model(sphere,sphere_tranform,WHITE);
        render_model(plane,plane_transform,WHITE);

        render_end();

        draw_editor_tools();
        DrawFPS(10,10);

        EndDrawing();
    }

    UnloadModel(sphere);
    UnloadModel(plane);

    render_close();

    close_editor_tools();

    CloseWindow();

    return 0;
}