#ifndef _FLUX_EDITOR_BUTTONS_H_
#define _FLUX_EDITOR_BUTTONS_H_

#include "raylib.h"
#include "coords.h"

struct editorButtonStruct;
typedef struct editorButtonStruct* editorButton;

void init_buttons(void);

void delete_buttons(void);

editorButton make_button(editorRect rect, Color color_base, Color color_hover, Color color_click, void (*onClick)(struct editorButtonStruct*), void (*onClickOff)(struct editorButtonStruct*), const char* text);

bool draw_button(editorButton button, Vector2 offset);

void button_stay_clicked(editorButton button);

void disable_button(editorButton button);

void enable_button(editorButton button);

#endif