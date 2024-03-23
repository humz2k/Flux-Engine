#ifndef _FLUX_ENGINE_CONSOLE_H_
#define _FLUX_ENGINE_CONSOLE_H_

#include <stdio.h>

typedef void (*flux_console_command_callback)(int,const char**);

void init_console(void);

void delete_console(void);

void update_console(void);

void init_stack(void);

void delete_stack(void);

void CustomLog(int msgType, const char *text, va_list args);

void add_console_command(const char* name, flux_console_command_callback callback);

#endif