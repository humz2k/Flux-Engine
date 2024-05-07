/**
 * @file top_tool_bar.c
 **/

#include "top_tool_bar.h"
#include "buttons.h"
#include "coords.h"
#include "editor_config.h"
#include "editor_theme.h"
#include "hqtools/hqtools.h"
#include "panels.h"
#include "raylib.h"
#include "raymath.h"
#include "text_stuff.h"
#include <stdio.h>
#include <stdlib.h>

static editorPanel top_tool_bar_panel;
static editorPanel file_panel;
static int current_button_pos = EDITOR_TOOL_BAR_SIZE;

static void file_click_callback(editorButton button) {
    TraceLog(LOG_FLUX_EDITOR, "file clicked");
    editor_enable_panel(file_panel);
}

static void file_click_off_callback(editorButton button) {
    TraceLog(LOG_FLUX_EDITOR, "file off clicked");
    if (editor_vector_in_panel(file_panel, GetMousePosition())) {
        editor_button_stay_clicked(button);
    } else {
        editor_disable_panel(file_panel);
    }
}

static void edit_click_callback(editorButton button) {
    TraceLog(LOG_FLUX_EDITOR, "edit clicked");
}

static void edit_click_off_callback(editorButton button) {
    TraceLog(LOG_FLUX_EDITOR, "edit off clicked");
}

static void view_click_callback(editorButton button) {
    TraceLog(LOG_FLUX_EDITOR, "view clicked");
}

static void view_click_off_callback(editorButton button) {
    TraceLog(LOG_FLUX_EDITOR, "view off clicked");
}

static void
tool_bar_add_button(const char* text,
                    void (*onClick)(struct editorButtonStruct*),
                    void (*onClickOff)(struct editorButtonStruct*)) {
    editorButton button = editor_make_button(
        make_rect(
            make_coord(make_pos_pixel(current_button_pos), make_pos_pixel(0)),
            make_coord(
                make_pos_pixel(EDITOR_TOOL_BAR_SIZE * 3 + current_button_pos),
                make_pos_pixel(EDITOR_TOOL_BAR_SIZE))),
        (Color){0, 0, 0, 0}, (Color){0, 0, 0, 20}, (Color){0, 0, 0, 50},
        onClick, onClickOff, text);

    editor_add_button_to_panel(top_tool_bar_panel, button);

    current_button_pos += EDITOR_TOOL_BAR_SIZE * 3;
}

void editor_init_top_tool_bar() {
    TraceLog(LOG_FLUX_EDITOR, "editor_init_top_tool_bar");
    top_tool_bar_panel = editor_make_editor_panel(
        EDITOR_TOP_TOOL_BAR_RECT, EDITOR_TOP_TOOL_BAR_COLOR, false,
        EDITOR_TOOL_BAR_SHADOW_SIZE, 0, 1, 0, 0, 0);

    file_panel = editor_make_editor_panel(
        make_rect(make_coord(make_pos_pixel(50), make_pos_pixel(50)),
                  make_coord(make_pos_pixel(200), make_pos_pixel(500))),
        GREEN, false, EDITOR_TOOL_BAR_SHADOW_SIZE, 0, 1, 0, 1, 2);
    editor_disable_panel(file_panel);

    tool_bar_add_button("File", file_click_callback, file_click_off_callback);
    tool_bar_add_button("Edit", edit_click_callback, edit_click_off_callback);
    tool_bar_add_button("View", view_click_callback, view_click_off_callback);
}

void editor_delete_top_tool_bar() {
    TraceLog(LOG_FLUX_EDITOR, "editor_delete_top_tool_bar");
}