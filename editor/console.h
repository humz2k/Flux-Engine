#ifndef _FLUX_ENGINE_CONSOLE_H_
#define _FLUX_ENGINE_CONSOLE_H_

#include <stdio.h>

typedef void (*flux_console_command_callback)(int,const char**);

void editor_init_console(void);

void editor_delete_console(void);

void editor_update_console(void);

void editor_init_stack(void);

void editor_delete_stack(void);

void CustomLog(int msgType, const char *text, va_list args);

void editor_add_console_command(const char* name, flux_console_command_callback callback);

bool editor_console_active(void);

#endif