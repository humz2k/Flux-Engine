#include "buttons.h"
#include "coords.h"
#include "editor_config.h"
#include "editor_theme.h"
#include "hqtools/hqtools.h"
#include "raylib.h"
#include "raymath.h"
#include "text_stuff.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct editorButtonStruct {
    editorRect rect;
    Color color_base;
    Color color_hover;
    Color color_click;
    bool last_click_was_me;
    bool enabled;
    char text[EDITOR_MAX_BUTTON_TEXT];
    void (*onClick)(struct editorButtonStruct*);
    void (*onClickOff)(struct editorButtonStruct*);
};

static struct editorButtonStruct** allocated_buttons = NULL;
static int n_allocated_buttons = 0;
static int n_buttons = 0;

void editor_init_buttons(void) {
    TraceLog(LOG_FLUX_EDITOR, "editor_init_buttons");
    n_buttons = 0;
    n_allocated_buttons = 10;
    assert(allocated_buttons == NULL);
    assert(allocated_buttons = (struct editorButtonStruct**)malloc(
               sizeof(struct editorButtonStruct*) * n_allocated_buttons));
}

void editor_delete_buttons(void) {
    TraceLog(LOG_FLUX_EDITOR, "editor_delete_buttons");
    for (int i = 0; i < n_buttons; i++) {
        free(allocated_buttons[i]);
    }
    n_allocated_buttons = 0;
    n_buttons = 0;
    free(allocated_buttons);
}

static void grow_buttons(void) {
    TraceLog(LOG_FLUX_EDITOR, "growing allocated buttons from %d to %d",
             n_allocated_buttons, n_allocated_buttons * 2);
    n_allocated_buttons *= 2;
    assert(n_allocated_buttons > 0);
    assert(allocated_buttons = (struct editorButtonStruct**)realloc(
               allocated_buttons,
               sizeof(struct editorButtonStruct*) * n_allocated_buttons));
}

editorButton editor_make_button(editorRect rect, Color color_base,
                                Color color_hover, Color color_click,
                                void (*onClick)(struct editorButtonStruct*),
                                void (*onClickOff)(struct editorButtonStruct*),
                                const char* text) {
    if (n_buttons >= n_allocated_buttons) {
        grow_buttons();
    }
    assert(n_buttons < n_allocated_buttons);
    allocated_buttons[n_buttons] =
        (struct editorButtonStruct*)malloc(sizeof(struct editorButtonStruct));
    editorButton out = allocated_buttons[n_buttons];
    n_buttons++;
    out->rect = rect;
    out->color_base = color_base;
    out->color_hover = color_hover;
    out->color_click = color_click;
    out->onClick = onClick;
    out->onClickOff = onClickOff;
    out->last_click_was_me = false;
    out->enabled = true;
    if (text) {
        assert(strlen(text) < EDITOR_MAX_BUTTON_TEXT);
        strcpy(out->text, text);
    } else {
        out->text[0] = '\0';
    }
    return out;
}

void editor_button_stay_clicked(editorButton button) {
    assert(button);
    button->last_click_was_me = true;
}

void editor_disable_button(editorButton button) {
    assert(button);
    button->enabled = 0;
}

void editor_enable_button(editorButton button) {
    assert(button);
    button->enabled = 1;
}

bool editor_draw_button(editorButton button, Vector2 offset) {
    assert(button);
    if (!button->enabled)
        return false;
    bool on_button = false;
    Color color = button->color_base;
    if (vector_in_editor_rect(button->rect, offset, GetMousePosition())) {
        on_button = true;
        color = button->color_hover;
        if (IsMouseButtonDown(0)) {
            color = button->color_click;
        }
        if (IsMouseButtonPressed(0)) {
            button->last_click_was_me = true;
            button->onClick(button);
        }
    } else {
        if (IsMouseButtonPressed(0) && button->last_click_was_me) {
            button->last_click_was_me = false;
            button->onClickOff(button);
        }
    }
    draw_editor_rect(button->rect, color, offset);
    draw_text_in_editor_rect(button->rect, offset, button->text, WHITE);
    return on_button;
}