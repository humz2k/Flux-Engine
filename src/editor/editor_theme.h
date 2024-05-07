/**
 * @file editor_theme.h
 **/

#ifndef _FLUX_EDITOR_THEME_H_
#define _FLUX_EDITOR_THEME_H_

#include "coords.h"
#include "raylib.h"

#define EDITOR_MAX_BUTTON_TEXT 100
#define EDITOR_PANEL_MAX_BUTTONS 50

#define EDITOR_BACKGROUND_COLOR ((Color){30, 35, 43, 255})
#define EDITOR_TOP_TOOL_BAR_COLOR ((Color){23, 27, 33, 255})
#define EDITOR_SIDE_TOOL_BAR_COLOR ((Color){50, 55, 64, 255})
#define EDITOR_STATUS_BAR_COLOR ((Color){27, 27, 30, 255})
#define EDITOR_WINDOW_COLOR ((Color){25, 25, 25, 255})

#define EDITOR_TOOL_BAR_SHADOW_SIZE make_pos_pixel(10)
#define EDITOR_TOP_TOOL_BAR_RECT                                               \
    make_rect(make_coord(make_pos_pixel(0), make_pos_pixel(0)),                \
              make_coord(make_pos_relative(1),                                 \
                         make_pos_pixel(EDITOR_TOOL_BAR_SIZE)))
#define EDITOR_SIDE_TOOL_BAR_RECT                                              \
    make_rect(                                                                 \
        make_coord(make_pos_pixel(0), make_pos_pixel(EDITOR_TOOL_BAR_SIZE)),   \
        make_coord(make_pos_pixel(EDITOR_TOOL_BAR_SIZE),                       \
                   make_pos_relative(1)))

#define EDITOR_TOOL_BAR_SIZE 25

#define EDITOR_FONT "src/editor/JetBrainsMono-Regular.ttf"
#define EDITOR_FONT_BASE_SIZE 128

#define CONSOLE_GL_COLOR GREEN
#define CONSOLE_RAYLIB_COLOR SKYBLUE
#define CONSOLE_EDITOR_COLOR YELLOW

#endif