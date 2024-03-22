#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "raylib.h"
#include "raymath.h"
#include "coords.h"
#include "panels.h"
#include "buttons.h"
#include "editor_theme.h"

struct editorPanelStruct{
    Vector2 pos;
    editorRect rect;
    editorPos shadow_size;
    Color color;
    bool top_shadow, bottom_shadow, left_shadow, right_shadow;
    bool draggable;
    bool was_clicked;
    int n_buttons;
    int enabled;
    int layer;
    editorButton buttons[EDITOR_PANEL_MAX_BUTTONS];
};

static struct editorPanelStruct* allocated_panels = NULL;
static int n_allocated_panels = 0;
static int n_panels = 0;

void init_panels(void){
    n_panels = 0;
    n_allocated_panels = 10;
    assert(allocated_panels == NULL);
    assert(allocated_panels = (struct editorPanelStruct*)malloc(sizeof(struct editorPanelStruct) * n_allocated_panels));
}

void delete_panels(void){
    n_allocated_panels = 0;
    n_panels = 0;
    free(allocated_panels);
}

static void grow_panels(void){
    TraceLog(LOG_INFO,"growing allocated panels from %d to %d",n_allocated_panels,n_allocated_panels * 2);
    n_allocated_panels *= 2;
    assert(n_allocated_panels > 0);
    assert(allocated_panels = (struct editorPanelStruct*)realloc(allocated_panels,sizeof(struct editorPanelStruct) * n_allocated_panels));
}

editorPanel make_editor_panel(editorRect rect, Color color, bool draggable, editorPos shadow_size, bool top_shadow, bool bottom_shadow, bool left_shadow, bool right_shadow, int layer){
    if (n_panels >= n_allocated_panels){
        grow_panels();
    }
    assert(n_panels < n_allocated_panels);
    editorPanel out = &allocated_panels[n_panels];
    n_panels++;
    out->draggable = draggable;
    out->pos = Vector2Zero();
    out->rect = rect;
    out->color = color;
    out->top_shadow = top_shadow;
    out->bottom_shadow = bottom_shadow;
    out->left_shadow = left_shadow;
    out->right_shadow = right_shadow;
    out->shadow_size = shadow_size;
    out->was_clicked = false;
    out->n_buttons = 0;
    out->enabled = 1;
    out->layer = layer;
    return out;
}

void add_button_to_panel(editorPanel panel, editorButton button){
    assert(panel);
    assert(button);
    assert(panel->n_buttons < EDITOR_PANEL_MAX_BUTTONS);
    panel->buttons[panel->n_buttons] = button;
    panel->n_buttons++;
}

void enable_panel(editorPanel panel){
    assert(panel);
    panel->enabled = 1;
}
void disable_panel(editorPanel panel){
    assert(panel);
    panel->enabled = 0;
}

void panel_set_layer(editorPanel panel, int layer){
    assert(panel);
    panel->layer = layer;
}

void draw_editor_panel(editorPanel panel){
    assert(panel);
    if (!panel->enabled)return;
    bool can_drag = false;
    draw_editor_rect_shadow(panel->rect,panel->color,panel->pos,panel->shadow_size,panel->top_shadow,panel->bottom_shadow,panel->left_shadow,panel->right_shadow);
    for (int i = 0; i < panel->n_buttons; i++){
        can_drag |= !draw_button(panel->buttons[i],panel->pos);
    }

    if ((panel->draggable) && can_drag){
        if (IsMouseButtonPressed(0) && vector_in_editor_rect(panel->rect,panel->pos,GetMousePosition()))
            panel->was_clicked = true;

        if (panel->was_clicked){
            if (!IsMouseButtonDown(0))
                panel->was_clicked = false;
            else
                panel->pos = Vector2Add(panel->pos,GetMouseDelta());
        }
    }
}

bool vector_in_panel(editorPanel panel, Vector2 vector){
    return vector_in_editor_rect(panel->rect,panel->pos,vector);
}

void draw_panels(void){
    for (int layer = 0; layer < 5; layer++){
        for (int i = 0; i < n_panels; i++){
            if (allocated_panels[i].layer == layer)
                draw_editor_panel(&allocated_panels[i]);
        }
    }
}
