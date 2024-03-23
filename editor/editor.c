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

Font editor_font;

void draw_status_bar(void){
    editorRect status_bar = make_rect(make_coord(make_pos_relative(0),make_pos_pixel(GetScreenHeight() - 20)),make_coord(make_pos_relative(1),make_pos_relative(1)));
    draw_editor_rect(status_bar,EDITOR_STATUS_BAR_COLOR,Vector2Zero());
    char fps_text[200];
    sprintf(fps_text,"fps %d",GetFPS());
    draw_text_in_editor_rect(make_rect(make_coord(make_pos_relative(0),make_pos_pixel(GetScreenHeight() - 20)),make_coord(make_pos_pixel(EDITOR_TOOL_BAR_SIZE*3),make_pos_relative(1))),Vector2Zero(),fps_text,WHITE);
    draw_text_in_editor_rect(status_bar,Vector2Zero(),EDITOR_VERSION_STRING,WHITE);
}

int main(){

    init_stack();
    SetTraceLogCallback(CustomLog);

    init_panels();
    init_buttons();
    init_top_tool_bar();
    init_input_boxes();
    init_console();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200,800,"flux_editor");
    SetTargetFPS(200);

    Texture2D logo = LoadTextureFromImage(LoadImage("/Users/humzaqureshi/GitHub/Flux-Engine/logo.png"));

    editor_font = LoadFontEx(EDITOR_FONT,EDITOR_FONT_BASE_SIZE,NULL,0);

    editorPanel side_tool_bar_panel = make_editor_panel(
                                        EDITOR_SIDE_TOOL_BAR_RECT,
                                        EDITOR_SIDE_TOOL_BAR_COLOR,
                                        false,
                                        EDITOR_TOOL_BAR_SHADOW_SIZE,
                                        0, 0, 0, 0, 1);

    while (!WindowShouldClose()){

        update_console();

        BeginDrawing();

        ClearBackground(EDITOR_BACKGROUND_COLOR);

        draw_panels();

        draw_status_bar();

        DrawTexturePro(logo,(Rectangle){0,0,1024,1024},(Rectangle){0,0,EDITOR_TOOL_BAR_SIZE,EDITOR_TOOL_BAR_SIZE},Vector2Zero(),0,WHITE);

        EndDrawing();
    }

    UnloadFont(editor_font);
    UnloadTexture(logo);

    delete_input_boxes();
    delete_console();
    delete_panels();
    delete_buttons();
    delete_stack();

    CloseWindow();
    return 0;
}