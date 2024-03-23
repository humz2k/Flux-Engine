#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "coords.h"
#include "buttons.h"
#include "panels.h"
#include "text_stuff.h"
#include "editor_theme.h"
#include "editor_config.h"
#include <time.h>

static editorPanel console_panel;

char console_input[EDITOR_TEXT_INPUT_MAX_CHARS];

static char* stack[EDITOR_CONSOLE_STACK_SIZE];
static int stack_ptr = 0;

void init_stack(void){
    for (int i = 0; i < EDITOR_CONSOLE_STACK_SIZE; i++){
        stack[i] = NULL;
    }
    stack_ptr = 0;
}

void delete_stack(void){
    for (int i = 0; i < EDITOR_CONSOLE_STACK_SIZE; i++){
        if (stack[i])free(stack[i]);
    }
}

static void append_stack(char* ptr){
    if (stack[stack_ptr])free(stack[stack_ptr]);
    stack[stack_ptr] = ptr;
    stack_ptr++;
    if (stack_ptr >= EDITOR_CONSOLE_STACK_SIZE){
        stack_ptr = 0;
    }
}

static const char* read_stack(int i){
    int idx = (stack_ptr - 1) - i;
    while (idx < 0){
        idx += EDITOR_CONSOLE_STACK_SIZE;
    }
    if (idx >= EDITOR_CONSOLE_STACK_SIZE)return NULL;
    return stack[idx];
}

void CustomLog(int msgType, const char *text, va_list args)
{
    char timeStr[64] = { 0 };
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    strftime(timeStr, sizeof(timeStr), "[%X]", tm_info);

    const char* log_type;

    switch (msgType)
    {
        case LOG_INFO:    log_type = "[INFO] : "; break;
        case LOG_ERROR:   log_type = "[ERROR]: "; break;
        case LOG_WARNING: log_type = "[WARN] : "; break;
        case LOG_DEBUG:   log_type = "[DEBUG]: "; break;
        //case LOG_USER:    log_type = "[USER] : "; break;
        default: log_type = ""; break;
    }

    char msg[512];

    //vprintf(text, args);
    vsprintf(msg,text,args);

    char* out;
    assert(out = (char*)malloc(sizeof(char) * (strlen(timeStr) + strlen(log_type) + strlen(msg) + 5)));
    sprintf(out,"%s %s%s",timeStr,log_type,msg);
    printf("%s\n",out);
    append_stack(out);
}

void set_console_input(char* input){
    strcpy(console_input,input);
    if (IsKeyPressed(KEY_ENTER)){
        console_input[0] = '\0';
        input[0] = '\0';
        TraceLog(LOG_INFO,"SUBMIT!");
    }
}

void get_console_input(char* input){
    strcpy(input,console_input);
}

static int line_count = 0;
static int line_start = 0;

void get_line_input(char* input){
    const char* line = read_stack(line_count);
    line_count++;
    if (line)
        strcpy(input,line);
    else
        input[0] = '\0';
}

void init_console(void){
    float text_size = 0.025;
    editorRect console_rect = make_rect(
                make_coord(make_pos_relative(0.1),make_pos_relative(0.1)),
                make_coord(make_pos_relative(0.9),make_pos_relative(0.9)));
    console_panel = make_editor_panel(
        console_rect, EDITOR_WINDOW_COLOR, true, make_pos_relative(0.01), 1,1,1,1,3
    );
    console_input[0] = '\0';
    add_text_input_box_to_panel(console_panel,
        make_text_input_box( make_rect(
            make_coord(make_pos_relative(0.1),make_pos_relative(0.9 - text_size)),
            make_coord(make_pos_relative(0.9),make_pos_relative(0.9))),
        (Color){15,15,15,255},GREEN,WHITE,set_console_input,get_console_input
    ));
    float start = 0.9 - text_size;
    while (start - 0.00001 > 0.1){
        //printf("AHH\n");
        add_text_input_box_to_panel(console_panel,
                make_text_box(make_rect(
                    make_coord(make_pos_relative(0.1),make_pos_relative(start-text_size)),
                    make_coord(make_pos_relative(0.9),make_pos_relative(start))),(Color){0,0,0,0},WHITE,get_line_input));
        start -= text_size;
    }
}

void update_console(void){
    if (IsKeyPressed(KEY_GRAVE)){
        toggle_panel(console_panel);
    }
    if (panel_enabled(console_panel)){
        if (IsKeyPressed(KEY_UP)){
            line_start++;
        }
        if (IsKeyPressed(KEY_DOWN)){
            line_start--;
        }
        line_count = line_start;
    }
}

void delete_console(void){

}