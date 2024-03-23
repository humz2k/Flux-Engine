#include "pipeline.h"
#include "editor.h"
#include "console.h"

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

    Camera3D cam;
    cam.fovy = 45;
    cam.position = Vector3One();
    cam.projection = CAMERA_PERSPECTIVE;
    cam.up = (Vector3){0,1,0};
    cam.target = Vector3Zero();

    while (!WindowShouldClose() && !do_quit){
        BeginDrawing();

        ClearBackground(BLACK);

        render_begin(cam);
        render_end();

        draw_editor_tools();
        DrawFPS(10,10);

        EndDrawing();
    }

    close_editor_tools();

    CloseWindow();

    return 0;
}