#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "raylib.h"
#include "raymath.h"
#include "coords.h"
#include "buttons.h"
#include "panels.h"
#include "text_stuff.h"
#include "editor_theme.h"
#include "editor_config.h"
#include "input_boxes.h"

struct editorTextInputBoxStruct{
    editorRect rect;
    Color background_color;
    Color edit_color;
    Color text_color;
    char current_input[EDITOR_TEXT_INPUT_MAX_CHARS];
    void (*setText)(char*,editorTextInputBox);
    void (*getText)(char*,editorTextInputBox);
    bool edit_mode;
    bool last_edit_mode;
    bool editable;
};

static struct editorTextInputBoxStruct** allocated_text_input_boxes = NULL;
static int n_allocated_text_input_boxes = 0;
static int n_text_input_boxes = 0;

void editor_init_input_boxes(void){
    TraceLog(LOG_FLUX_EDITOR,"init_input_boxes");
    n_text_input_boxes = 0;
    n_allocated_text_input_boxes = 10;
    assert(allocated_text_input_boxes == NULL);
    assert(allocated_text_input_boxes = (struct editorTextInputBoxStruct**)malloc(sizeof(struct editorTextInputBoxStruct*) * n_allocated_text_input_boxes));
}

void editor_delete_input_boxes(void){
    TraceLog(LOG_FLUX_EDITOR,"delete_input_boxes");
    for (int i = 0; i < n_text_input_boxes; i++){
        free(allocated_text_input_boxes[i]);
    }
    n_allocated_text_input_boxes = 0;
    n_text_input_boxes = 0;
    free(allocated_text_input_boxes);
}

static void grow_text_input_boxes(void){
    TraceLog(LOG_FLUX_EDITOR,"growing allocated text_input_boxes from %d to %d",n_allocated_text_input_boxes,n_allocated_text_input_boxes * 2);
    n_allocated_text_input_boxes *= 2;
    assert(n_allocated_text_input_boxes > 0);
    assert(allocated_text_input_boxes = (struct editorTextInputBoxStruct**)realloc(allocated_text_input_boxes,sizeof(struct editorTextInputBoxStruct*) * n_allocated_text_input_boxes));
}

editorTextInputBox editor_make_text_input_box(editorRect rect, Color background_color, Color edit_color, Color text_color, void (*setText)(char*,editorTextInputBox), void (*getText)(char*,editorTextInputBox)){
    if (n_text_input_boxes >= n_allocated_text_input_boxes){
        grow_text_input_boxes();
    }
    allocated_text_input_boxes[n_text_input_boxes] = malloc(sizeof(struct editorTextInputBoxStruct));
    editorTextInputBox out = allocated_text_input_boxes[n_text_input_boxes];
    n_text_input_boxes++;
    out->rect = rect;
    out->background_color = background_color;
    out->edit_color = edit_color;
    out->text_color = text_color;
    out->setText = setText;
    out->getText = getText;
    out->edit_mode = false;
    out->last_edit_mode = false;
    out->current_input[0] = '\0';
    out->editable = true;
    return out;
}

editorTextBox editor_make_text_box(editorRect rect, Color background_color, Color text_color, void (*getText)(char*,editorTextBox)){
    if (n_text_input_boxes >= n_allocated_text_input_boxes){
        grow_text_input_boxes();
    }
    allocated_text_input_boxes[n_text_input_boxes] = malloc(sizeof(struct editorTextInputBoxStruct));
    editorTextBox out = allocated_text_input_boxes[n_text_input_boxes];
    n_text_input_boxes++;
    out->rect = rect;
    out->background_color = background_color;
    out->text_color = text_color;
    out->getText = getText;
    out->current_input[0] = '\0';
    out->editable = false;
    out->edit_mode = false;
    out->last_edit_mode = false;
    return out;
}

void editor_set_text_box_text_color(editorTextBox box, Color text_color){
    assert(box);
    box->text_color = text_color;
}

bool editor_draw_text_input_box(editorTextInputBox box, Vector2 offset){
    assert(box);
    bool not_draggable = false;
    if (box->editable){
        if (IsMouseButtonPressed(0)){
            if (vector_in_editor_rect(box->rect,offset,GetMousePosition())){
                box->edit_mode = true;
                not_draggable = true;
            } else {
                box->edit_mode = false;
                if (box->last_edit_mode){
                    box->setText(box->current_input,box);
                }
            }
        }
    }
    Color col;
    if (box->edit_mode){
        if (IsKeyPressed(KEY_BACKSPACE)){
            int len = strlen(box->current_input);
            if (len >= 1)len -= 1;
            box->current_input[len] = '\0';
        } else {
            int key = GetCharPressed();
            if ((key >= 32) && (key <= 125) && (key != KEY_GRAVE)){
                int len = strlen(box->current_input);
                if (len < (EDITOR_MAX_BUTTON_TEXT - 1)){
                    box->current_input[len] = (char)key;
                    box->current_input[len+1] = '\0';
                }
            }
        }
        if (IsKeyPressed(KEY_ENTER)){
            box->setText(box->current_input,box);
        }
        col = box->edit_color;
    } else {
        box->getText(box->current_input,box);
        col = box->background_color;
    }

    draw_editor_rect(box->rect,col,offset);
    draw_text_in_editor_rect_left_aligned(box->rect,offset,box->current_input,box->text_color);

    box->last_edit_mode = box->edit_mode;
    return not_draggable;
}

void editor_set_text_input_box_edit_mode(editorTextInputBox box, bool mode){
    assert(box);
    assert(box->editable);
    box->edit_mode = mode;
}