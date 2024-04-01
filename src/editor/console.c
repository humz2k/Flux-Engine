#include "console.h"
#include "buttons.h"
#include "coords.h"
#include "editor_config.h"
#include "editor_theme.h"
#include "hqtools/hqtools.h"
#include "panels.h"
#include "raylib.h"
#include "raymath.h"
#include "text_stuff.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_USER 12

static editorPanel console_panel;
static editorTextInputBox console_input_box;

struct console_command {
    char name[EDITOR_CONSOLE_COMMAND_MAX_NAME];
    flux_console_command_callback callback;
};

static int n_commands = 0;
static struct console_command commands[EDITOR_CONSOLE_MAX_COMMANDS];

static char console_input[EDITOR_TEXT_INPUT_MAX_CHARS];
static char* stack[EDITOR_CONSOLE_STACK_SIZE];
static int message_type_stack[EDITOR_CONSOLE_STACK_SIZE];
static int stack_ptr = 0;

void editor_init_stack(void) {
    for (int i = 0; i < EDITOR_CONSOLE_STACK_SIZE; i++) {
        stack[i] = NULL;
    }
    stack_ptr = 0;
    hq_allocator_init_global();
}

void editor_delete_stack(void) {
    for (int i = 0; i < EDITOR_CONSOLE_STACK_SIZE; i++) {
        if (stack[i])
            free(stack[i]);
    }
    hq_allocator_delete_global();
}

static void append_stack(char* ptr, int type) {
    if (stack[stack_ptr])
        free(stack[stack_ptr]);
    stack[stack_ptr] = ptr;
    message_type_stack[stack_ptr] = type;
    stack_ptr++;
    if (stack_ptr >= EDITOR_CONSOLE_STACK_SIZE) {
        stack_ptr = 0;
    }
}

static const char* read_stack(int i, int* type) {
    int idx = (stack_ptr - 1) - i;
    while (idx < 0) {
        idx += EDITOR_CONSOLE_STACK_SIZE;
    }
    if (idx >= EDITOR_CONSOLE_STACK_SIZE)
        return NULL;
    *type = message_type_stack[idx];
    return stack[idx];
}

void CustomLog(int msgType, const char* text, va_list args) {

    const char* log_type;

    switch (msgType) {
    case LOG_INFO:
        log_type = "[INFO] : ";
        break;
    case LOG_ERROR:
        log_type = "[ERROR]: ";
        break;
    case LOG_WARNING:
        log_type = "[WARN] : ";
        break;
    case LOG_DEBUG:
        log_type = "[DEBUG]: ";
        break;
    case LOG_USER:
        log_type = "[USER] : ";
        break;
    case LOG_FLUX_EDITOR:
        log_type = "[INFO] : EDITOR: ";
        msgType = LOG_INFO;
        break;
    case LOG_FLUX_EDITOR_WARNING:
        log_type = "[WARN] : EDITOR: ";
        msgType = LOG_WARNING;
        break;
    case LOG_FLUX_EDITOR_ERROR:
        log_type = "[ERROR] : EDITOR: ";
        msgType = LOG_ERROR;
        break;
    default:
        log_type = "";
        break;
    }

    char msg[512];

    vsprintf(msg, text, args);

    char* out;
    assert(out = (char*)malloc(sizeof(char) *
                               (strlen(log_type) + strlen(msg) + 5)));
    sprintf(out, "%s%s", log_type, msg);
    printf("%s\n", out);
    append_stack(out, msgType);
}

static int n_command_items(const char* command) {
    char raw_command[strlen(command) + 5];
    strcpy(raw_command, command);
    char* token = strtok(raw_command, " ");
    int count = 0;
    while (token) {
        token = strtok(NULL, " ");
        count++;
    }
    return count;
}

static struct console_command* find_command(const char* name) {
    for (int i = 0; i < n_commands; i++) {
        if (strcmp(commands[i].name, name) == 0) {
            return &commands[i];
        }
    }
    return NULL;
}

static void parse_command(const char* command) {
    TraceLog(LOG_USER, command);

    int n_args = n_command_items(command);
    if (n_args == 0)
        return;

    const char* args[n_args];
    char raw_command[strlen(command) + 5];
    strcpy(raw_command, command);

    char* token = strtok(raw_command, " ");
    for (int i = 0; i < n_args; i++) {
        args[i] = token;
        token = strtok(NULL, " ");
    }

    const char* name = args[0];
    struct console_command* console_command = find_command(name);
    if (!console_command) {
        TraceLog(LOG_FLUX_EDITOR_WARNING, "invalid command");
        return;
    }
    console_command->callback(n_args, args);
}

static void set_console_input(char* input, editorTextInputBox box) {
    strcpy(console_input, input);
    if (IsKeyPressed(KEY_ENTER)) {
        parse_command(console_input);
        console_input[0] = '\0';
        input[0] = '\0';
    }
}

static void get_console_input(char* input, editorTextInputBox box) {
    strcpy(input, console_input);
}

static int line_count = 0;
static int line_start = 0;

static void get_line_input(char* input, editorTextBox box) {
    int type;
    const char* line = read_stack(line_count, &type);
    line_count++;
    if (!line) {
        input[0] = '\0';
        return;
    }
    Color col = WHITE;

    if (strstr(line, "GL:") || strstr(line, "GLAD:")) {
        col = CONSOLE_GL_COLOR;
    } else if (strstr(line, "TEXTURE:") || strstr(line, "DISPLAY:") ||
               strstr(line, "PLATFORM:") || strstr(line, "SHADER:") ||
               strstr(line, "RLGL:") || strstr(line, "FONT:") ||
               strstr(line, "TIMER:") || strstr(line, "FILEIO:") ||
               strstr(line, "IMAGE:")) {
        col = CONSOLE_RAYLIB_COLOR;
    } else if (strstr(line, "EDITOR:")) {
        col = CONSOLE_EDITOR_COLOR;
    }

    switch (type) {
    case LOG_WARNING:
        col = ORANGE;
        break;
    case LOG_ERROR:
        col = RED;
        break;
    default:
        break;
    }

    editor_set_text_box_text_color(box, col);
    if (line)
        strcpy(input, line);
}

void editor_add_console_command(const char* name,
                                flux_console_command_callback callback) {
    assert(n_commands < EDITOR_CONSOLE_MAX_COMMANDS);
    assert(strlen(name) < EDITOR_CONSOLE_COMMAND_MAX_NAME);
    TraceLog(LOG_FLUX_EDITOR, "adding console command %s", name);
    strcpy(commands[n_commands].name, name);
    commands[n_commands].callback = callback;
    n_commands++;
}

static void test_console_command(int n_args, const char** args) {
    TraceLog(LOG_FLUX_EDITOR, "testing!");
}

void editor_init_console(void) {
    TraceLog(LOG_FLUX_EDITOR, "init_console");
    float text_size = 0.017;
    editorRect console_rect =
        make_rect(make_coord(make_pos_relative(0.1), make_pos_relative(0.1)),
                  make_coord(make_pos_relative(0.9), make_pos_relative(0.9)));
    console_panel =
        editor_make_editor_panel(console_rect, EDITOR_WINDOW_COLOR, true,
                                 make_pos_relative(0.01), 1, 1, 1, 1, 3);
    console_input[0] = '\0';
    console_input_box = editor_make_text_input_box(
        make_rect(make_coord(make_pos_relative(0.1),
                             make_pos_relative(0.9 - text_size)),
                  make_coord(make_pos_relative(0.9), make_pos_relative(0.9))),
        (Color){15, 15, 15, 255}, GREEN, WHITE, set_console_input,
        get_console_input);
    editor_add_text_input_box_to_panel(console_panel, console_input_box);
    float start = 0.9 - text_size;
    while (start - 0.01 > 0.1) {
        // printf("AHH\n");
        editor_add_text_input_box_to_panel(
            console_panel,
            editor_make_text_box(
                make_rect(make_coord(make_pos_relative(0.1),
                                     make_pos_relative(start - text_size)),
                          make_coord(make_pos_relative(0.9),
                                     make_pos_relative(start))),
                (Color){0, 0, 0, 0}, WHITE, get_line_input));
        start -= text_size;
    }
    n_commands = 0;

    editor_add_console_command("test", test_console_command);

    editor_set_text_input_box_edit_mode(console_input_box, true);
    editor_disable_panel(console_panel);
}

void editor_update_console(void) {
    if (IsKeyPressed(KEY_GRAVE)) {
        editor_toggle_panel(console_panel);
        if (editor_panel_enabled(console_panel)) {
            editor_set_text_input_box_edit_mode(console_input_box, true);
        }
    }
    if (editor_panel_enabled(console_panel)) {
        if (IsKeyPressed(KEY_UP)) {
            line_start++;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            line_start--;
        }
        line_count = line_start;
    }
}

void editor_delete_console(void) {
    TraceLog(LOG_FLUX_EDITOR, "editor_delete_console");
}

bool editor_console_active(void) { return editor_panel_enabled(console_panel); }