#ifndef _FLUX_EDITOR_INPUT_BOXES_H_
#define _FLUX_EDITOR_INPUT_BOXES_H_

#include "raylib.h"
#include "coords.h"

struct editorTextInputBoxStruct;
typedef struct editorTextInputBoxStruct* editorTextInputBox;
typedef editorTextInputBox editorTextBox;

void init_input_boxes(void);

void delete_input_boxes(void);

editorTextInputBox make_text_input_box(editorRect rect, Color background_color, Color edit_color, Color text_color, void (*setText)(char*), void (*getText)(char*));

editorTextBox make_text_box(editorRect rect, Color background_color, Color text_color, void (*getText)(char*));

bool draw_text_input_box(editorTextInputBox box, Vector2 offset);

#endif