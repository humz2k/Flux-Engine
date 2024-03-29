#ifndef _FLUX_EDITOR_PANELS_H_
#define _FLUX_EDITOR_PANELS_H_

#include "raylib.h"
#include "coords.h"
#include "buttons.h"
#include "input_boxes.h"

struct editorPanelStruct;
typedef struct editorPanelStruct* editorPanel;

void editor_init_panels(void);

void editor_delete_panels(void);

editorPanel editor_make_editor_panel(editorRect rect, Color color, bool draggable, editorPos shadow_size, bool top_shadow, bool bottom_shadow, bool left_shadow, bool right_shadow, int layer);

void editor_draw_editor_panel(editorPanel panel);

void editor_add_button_to_panel(editorPanel panel, editorButton button);

void editor_add_text_input_box_to_panel(editorPanel panel, editorTextInputBox text_input_box);

void editor_enable_panel(editorPanel panel);

void editor_disable_panel(editorPanel panel);

bool editor_vector_in_panel(editorPanel panel, Vector2 vector);

void editor_draw_panels(void);

void editor_panel_set_layer(editorPanel panel, int layer);

void editor_toggle_panel(editorPanel panel);

bool editor_panel_enabled(editorPanel panel);

#endif