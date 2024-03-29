#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "raylib.h"
#include "editor_config.h"
#include "console.h"
#include "hqtools/hqtools.h"

static void console_command_pwd(int nargs, const char** args){
    TraceLog(LOG_FLUX_EDITOR,GetWorkingDirectory());
}

static void console_command_cd(int nargs, const char** args){
    assert(nargs >= 2);
    ChangeDirectory(args[1]);
}

static void console_command_ls(int nargs, const char** args){
    FilePathList files = LoadDirectoryFiles(GetWorkingDirectory());
    int size = 0;
    char out[EDITOR_CONSOLE_COMMAND_MAX_NAME]; out[0] = '\0';
    for (int i = 0; i < files.count; i++){
        strcat(out,GetFileName(files.paths[i]));
        if (strlen(out) >= 45){
            TraceLog(LOG_FLUX_EDITOR,out);
            out[0] = '\0';
        } else {
            strcat(out," ");
        }
        //printf("%s\n",files.paths[i]);
    }
    TraceLog(LOG_FLUX_EDITOR,out);

}

void editor_init_filesys_tools(){
    TraceLog(LOG_FLUX_EDITOR,"editor_init_filesys_tools");
    editor_add_console_command("pwd",console_command_pwd);
    editor_add_console_command("cd",console_command_cd);
    editor_add_console_command("ls",console_command_ls);
}

void editor_delete_filesys_tools(){
    TraceLog(LOG_FLUX_EDITOR,"editor_delete_filesys_tools");
}