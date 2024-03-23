#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "editor_theme.h"
#include "coords.h"
#include "panels.h"
#include "buttons.h"
#include "text_stuff.h"
#include "top_tool_bar.h"
#include "editor_config.h"
#include "console.h"
#include "input_boxes.h"
#include "filesys_tools.h"
#include "editor.h"

Font editor_font;
static bool do_quit = false;

void draw_status_bar(void){
    editorRect status_bar = make_rect(make_coord(make_pos_relative(0),make_pos_pixel(GetScreenHeight() - 20)),make_coord(make_pos_relative(1),make_pos_relative(1)));
    draw_editor_rect(status_bar,EDITOR_STATUS_BAR_COLOR,Vector2Zero());
    char fps_text[200];
    sprintf(fps_text,"fps %d",GetFPS());
    draw_text_in_editor_rect(make_rect(make_coord(make_pos_relative(0),make_pos_pixel(GetScreenHeight() - 20)),make_coord(make_pos_pixel(EDITOR_TOOL_BAR_SIZE*3),make_pos_relative(1))),Vector2Zero(),fps_text,WHITE);
    draw_text_in_editor_rect(status_bar,Vector2Zero(),EDITOR_VERSION_STRING,WHITE);
}

void quit_editor(void){
    do_quit = true;
}

static void console_command_quit(int n_args, const char** args){
    TraceLog(LOG_FLUX_EDITOR,"goodbye");
    quit_editor();
}

void init_editor_tools(void){
    editor_init_stack();
    SetTraceLogCallback(CustomLog);
    editor_init_panels();
    editor_init_buttons();
    editor_init_input_boxes();
    editor_init_console();
}

void close_editor_tools(void){
    editor_delete_input_boxes();
    editor_delete_console();
    editor_delete_panels();
    editor_delete_buttons();
    editor_delete_stack();
}

void draw_editor_tools(void){
    editor_update_console();
    editor_draw_panels();
}

void load_editor_tools(void){
    editor_font = LoadFontEx(EDITOR_FONT,EDITOR_FONT_BASE_SIZE,NULL,0);
}

int run_editor(){

    init_editor_tools();

    editor_init_top_tool_bar();
    editor_init_filesys_tools();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200,800,"flux_editor");
    SetTargetFPS(200);

    Texture2D logo = LoadTextureFromImage(LoadImage("/Users/humzaqureshi/GitHub/Flux-Engine/logo.png"));

    load_editor_tools();

    editorPanel side_tool_bar_panel = editor_make_editor_panel(
                                        EDITOR_SIDE_TOOL_BAR_RECT,
                                        EDITOR_SIDE_TOOL_BAR_COLOR,
                                        false,
                                        EDITOR_TOOL_BAR_SHADOW_SIZE,
                                        0, 0, 0, 0, 1);

    editor_add_console_command("quit",console_command_quit);

    while (!WindowShouldClose() && !do_quit){

        BeginDrawing();

        ClearBackground(EDITOR_BACKGROUND_COLOR);

        draw_editor_tools();

        draw_status_bar();

        DrawTexturePro(logo,(Rectangle){0,0,1024,1024},(Rectangle){0,0,EDITOR_TOOL_BAR_SIZE,EDITOR_TOOL_BAR_SIZE},Vector2Zero(),0,WHITE);

        EndDrawing();
    }

    UnloadFont(editor_font);
    UnloadTexture(logo);

    editor_delete_top_tool_bar();
    editor_delete_filesys_tools();

    close_editor_tools();

    CloseWindow();
    return 0;
}