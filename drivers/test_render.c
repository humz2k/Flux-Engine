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
static Camera3D active_cam;
static Camera3D cam;
static int cam_id = -1;

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

static void debug_light_callback(int n_args, const char** args){
    if (n_args < 2)return;
    cam_id = atoi(args[1]);
    //active_cam = render_get_light_cam(atoi(args[1]));
}

static void default_cam_callback(int n_args, const char** args){
    cam_id = -1;
    //active_cam = cam;
}

static void light_set_scale_callback(int n_args, const char** args){
    if (n_args < 3)return;
    render_light_set_scale(atoi(args[1]),atof(args[2]));
}

static void light_set_fov_callback(int n_args, const char** args){
    if (n_args < 3)return;
    render_light_set_fov(atoi(args[1]),atof(args[2]));
}

int main(){

    init_editor_tools();

    //SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1200,800,"flux_render_test");
    //ToggleFullscreen();

    SetExitKey(0);

    load_editor_tools();

    editor_add_console_command("quit",quit_callback);
    editor_add_console_command("draw_grid",draw_grid_callback);
    editor_add_console_command("camera_mode",camera_mode_callback);
    //editor_add_console_command("fullscreen",fullscreen_callback);
    editor_add_console_command("query_res",query_res_callback);
    editor_add_console_command("debug_light",debug_light_callback);
    editor_add_console_command("default_cam",default_cam_callback);
    editor_add_console_command("light_set_scale",light_set_scale_callback);
    editor_add_console_command("light_set_fov",light_set_fov_callback);

    SetTargetFPS(200);

    int true_render_width = GetDisplayWidth() * 2;
    int true_render_height = GetDisplayHeight() * 2;

    RenderTexture2D tex = LoadRenderTexture(true_render_width,true_render_height);

    render_init();

    cam.fovy = 45;
    cam.position = (Vector3){0,10,-10};
    cam.projection = CAMERA_PERSPECTIVE;
    cam.up = (Vector3){0,1,0};
    cam.target = Vector3Zero();

    Model sphere = LoadModel("/Users/humzaqureshi/GitHub/Flux-Engine/drivers/assets/earth.obj");//LoadModelFromMesh(GenMeshSphere(1,10,10));
    fluxTransform sphere_tranform = flux_empty_transform();
    sphere_tranform.pos.y = 1;

    Model plane = LoadModelFromMesh(GenMeshPlane(50,50,10,10));
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
    render_light_set_L(1,(Vector3){0,2,-1});
    render_light_set_p(1,200);

    render_load_skybox("/Users/humzaqureshi/GitHub/Flux-Engine/drivers/assets/Daylight Box UV.png");

    active_cam = cam;

    renderModel sphere_rmodel = render_make_model(sphere);
    renderModel plane_model = render_make_model(plane);

    double frameStart = 0;
    double frameEnd = 0;

    while (!WindowShouldClose() && !do_quit){

        frameStart = GetTime();

        BeginDrawing();

        ClearBackground(BLACK);

        if (!editor_console_active()){
            if (camera_mode == 1){
                UpdateCamera(&cam,CAMERA_ORBITAL);
            } else if (camera_mode == 2){
                UpdateCamera(&cam,CAMERA_THIRD_PERSON);
            }
        }

        if (cam_id < 0){
            active_cam = cam;
        } else {
            active_cam = render_get_light_cam(cam_id);
        }

        render_begin(active_cam);

        render_reset_instances(sphere_rmodel);
        //render_model(thing,flux_empty_transform(),WHITE);
        sphere_tranform.pos.x = -5;
        for (int i = 0; i < 5; i++){
            sphere_tranform.pos.z = -5;
            for (int j = 0; j < 5; j++){
                render_add_model_instance(sphere_rmodel,sphere_tranform);
                //render_model(sphere,sphere_tranform,WHITE);
                sphere_tranform.pos.z += 2;
            }
            sphere_tranform.pos.x += 2;
        }

        render_rmodel(sphere_rmodel,WHITE);

        render_reset_instances(plane_model);
        render_add_model_instance(plane_model,plane_transform);
        render_rmodel(plane_model,WHITE);

        //render_model(plane,plane_transform,WHITE);

        if (draw_grid){
            render_draw_grid(100,1.0f);
        }

        render_calculate_shadows();
        BeginTextureMode(tex);
        render_end();
        EndTextureMode();

        DrawTexturePro(tex.texture,(Rectangle){0,0,true_render_width,-true_render_height},(Rectangle){0,0,GetDisplayWidth(),GetDisplayHeight()},Vector2Zero(),0,WHITE);

        draw_editor_tools();
        DrawFPS(10,10);
        char frame_time_str[200];
        sprintf(frame_time_str,"%f %f",GetFrameTime(),frameStart - frameEnd);
        DrawText(frame_time_str,30,30,10,GREEN);
        //GetFrameTime();

        EndDrawing();

        frameEnd = frameStart;
    }
    render_free_model(sphere_rmodel);
    render_free_model(plane_model);
    UnloadModel(sphere);
    UnloadModel(plane);
    UnloadRenderTexture(tex);

    render_close();

    CloseWindow();

    close_editor_tools();

    return 0;
}