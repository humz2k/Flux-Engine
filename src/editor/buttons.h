#ifndef _FLUX_EDITOR_BUTTONS_H_
#define _FLUX_EDITOR_BUTTONS_H_

#include "coords.h"
#include "raylib.h"

struct editorButtonStruct;
typedef struct editorButtonStruct* editorButton;

void editor_init_buttons(void);

void editor_delete_buttons(void);

editorButton editor_make_button(editorRect rect, Color color_base,
                                Color color_hover, Color color_click,
                                void (*onClick)(struct editorButtonStruct*),
                                void (*onClickOff)(struct editorButtonStruct*),
                                const char* text);

bool editor_draw_button(editorButton button, Vector2 offset);

void editor_button_stay_clicked(editorButton button);

void editor_disable_button(editorButton button);

void editor_enable_button(editorButton button);

#endif