#ifndef _FLUX_EDITOR_PANELS_H_
#define _FLUX_EDITOR_PANELS_H_

#include "raylib.h"
#include "coords.h"
#include "buttons.h"

struct editorPanelStruct;
typedef struct editorPanelStruct* editorPanel;

void init_panels(void);

void delete_panels(void);

editorPanel make_editor_panel(editorRect rect, Color color, bool draggable, editorPos shadow_size, bool top_shadow, bool bottom_shadow, bool left_shadow, bool right_shadow, int layer);

void draw_editor_panel(editorPanel panel);

void add_button_to_panel(editorPanel panel, editorButton button);

void enable_panel(editorPanel panel);

void disable_panel(editorPanel panel);

bool vector_in_panel(editorPanel panel, Vector2 vector);

void draw_panels(void);

void panel_set_layer(editorPanel panel, int layer);

#endif