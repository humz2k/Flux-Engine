#ifndef _FLUX_FILE_TOOLS_H_
#define _FLUX_FILE_TOOLS_H_

#include "hqtools/hqtools.h"
#include <stdio.h>
#include <stdlib.h>

static size_t get_file_length(FILE* fptr) {
    assert(fptr);
    fseek(fptr, 0L, SEEK_END);
    size_t sz = ftell(fptr);
    rewind(fptr);
    return sz;
}

static hstr read_whole_file(FILE* fptr) {
    hstr file_str = hstr_new("");

    char buffer[100];
    while (fgets(buffer, 100, fptr)) {
        file_str = hstr_concat(file_str, hstr_new(buffer));
    }
    return file_str;
}

#endif