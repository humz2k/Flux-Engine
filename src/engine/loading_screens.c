#include "loading_screens.h"
#include "raylib.h"
#include "text_stuff.h"
#include "display_size.h"

static int last = -1;

void flux_draw_loading_screen(const char* text, float amount){
    int next = (amount * 100.0f);
    if (next == last)return;
    TraceLog(LOG_INFO,"drawing loading screen");
    last = next;

    BeginDrawing();
    ClearBackground(BLACK);
    char out[1000];
    sprintf(out,"Loading %s...", text);
    DrawTextEx(editor_font, out, (Vector2){10.0, 10.0}, 50, 1,
               WHITE);

    float width = GetDisplayWidth();
    float height = GetDisplayHeight();

    float loading_bar_width = 0.7;
    float loading_bar_height = 0.2;

    int start_y = height * 0.5;
    int start_x = ((1.0f - loading_bar_width) * 0.5) * width;

    DrawRectangle(start_x,start_y,width * loading_bar_width * amount,height * loading_bar_height,WHITE);
    DrawRectangleLines(start_x,start_y,width * loading_bar_width,height * loading_bar_height,BLACK);
    EndDrawing();
}