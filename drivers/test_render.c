#include <stdlib.h>
#include <stdio.h>
#include "pipeline.h"
#include "editor.h"
#include "console.h"
#include "transform.h"
#include "display_size.h"

static bool do_quit = false;
static bool draw_grid = false;
static int camera_mode = 0;
static int fullscreen = 1;

static void quit_callback(int n_args, const char** args){
    do_quit = true;
}

static void draw_grid_callback(int n_args, const char** args){
    if (n_args < 2)return;
    draw_grid = atoi(args[1]);
}

static void camera_mode_callback(int n_args, const char** args){
    if (n_args < 2)return;
    camera_mode = atoi(args[1]);
}

static void fullscreen_callback(int n_args, const char** args){
    if (n_args < 2)return;
    int new_fullscreen = atoi(args[1]);
    if (new_fullscreen < 0)return;
    if (new_fullscreen > 1)return;
    if (new_fullscreen == fullscreen)return;
    fullscreen = new_fullscreen;
    ToggleFullscreen();
    if (fullscreen == 1){
        SetWindowSize(GetRenderWidth(),GetRenderHeight());
    }
}

static void query_res_callback(int n_args, const char** args){
    TraceLog(LOG_INFO,"screenSize = %dx%d, renderSize = %dx%d",GetScreenWidth(),GetScreenHeight(),GetRenderWidth(),GetRenderHeight());
}

int main(){

    init_editor_tools();

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1200,800,"flux_render_test");

    SetExitKey(0);

    load_editor_tools();

    editor_add_console_command("quit",quit_callback);
    editor_add_console_command("draw_grid",draw_grid_callback);
    editor_add_console_command("camera_mode",camera_mode_callback);
    //editor_add_console_command("fullscreen",fullscreen_callback);
    editor_add_console_command("query_res",query_res_callback);

    SetTargetFPS(200);

    int true_render_width = GetDisplayWidth() * 2;
    int true_render_height = GetDisplayHeight() * 2;

    RenderTexture2D tex = LoadRenderTexture(true_render_width,true_render_height);

    render_init();

    Camera3D cam;
    cam.fovy = 45;
    cam.position = (Vector3){0,2,-10};
    cam.projection = CAMERA_PERSPECTIVE;
    cam.up = (Vector3){0,1,0};
    cam.target = Vector3Zero();

    Model sphere = LoadModel("/Users/humzaqureshi/GitHub/Flux-Engine/drivers/assets/earth.obj");
    fluxTransform sphere_tranform = flux_empty_transform();
    sphere_tranform.pos.y = 1;

    Model plane = LoadModelFromMesh(GenMeshPlane(10,10,10,10));
    fluxTransform plane_transform = flux_empty_transform();

    //Model thing = LoadModel("/Users/humzaqureshi/GitHub/Flux-Engine/drivers/assets/map2.obj");

    render_set_ka(0.2);
    render_light_enable(0);
    render_light_set_type(0,0);
    render_light_set_cL(0,WHITE);
    render_light_set_kd(0,0.7);
    render_light_set_ks(0,0.3);
    render_light_set_L(0,Vector3One());
    render_light_set_p(0,200);

    render_light_enable(1);
    render_light_set_type(1,0);
    render_light_set_cL(1,WHITE);
    render_light_set_kd(1,0.7);
    render_light_set_ks(1,0.3);
    render_light_set_L(1,(Vector3){0,2,-0.5});
    render_light_set_p(1,200);

    render_load_skybox("/Users/humzaqureshi/GitHub/Flux-Engine/drivers/assets/Daylight Box UV.png");

    while (!WindowShouldClose() && !do_quit){
        BeginDrawing();

        ClearBackground(BLACK);

        if (!editor_console_active()){
            if (camera_mode == 1){
                UpdateCamera(&cam,CAMERA_ORBITAL);
            } else if (camera_mode == 2){
                UpdateCamera(&cam,CAMERA_THIRD_PERSON);
            }
        }

        BeginTextureMode(tex);
        render_begin(cam);

        //render_model(thing,flux_empty_transform(),WHITE);

        render_model(sphere,sphere_tranform,WHITE);
        render_model(plane,plane_transform,WHITE);

        if (draw_grid){
            render_draw_grid(100,1.0f);
        }

        render_end();
        EndTextureMode();

        DrawTexturePro(tex.texture,(Rectangle){0,0,true_render_width,true_render_height},(Rectangle){0,0,GetDisplayWidth(),GetDisplayHeight()},Vector2Zero(),0,WHITE);

        draw_editor_tools();
        DrawFPS(10,10);

        EndDrawing();
    }

    UnloadModel(sphere);
    UnloadModel(plane);
    UnloadRenderTexture(tex);

    render_close();

    CloseWindow();

    close_editor_tools();

    return 0;
}