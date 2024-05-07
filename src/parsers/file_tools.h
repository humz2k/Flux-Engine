/**
 * @file file_tools.h
 * @brief Provides utility functions for file handling, including reading files and obtaining file lengths.
 */

#ifndef _FLUX_FILE_TOOLS_H_
#define _FLUX_FILE_TOOLS_H_

#include "hqtools/hqtools.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Calculates the length of a file in bytes.
 *
 * Seeks to the end of the file, uses ftell to get the current position which represents the total file size,
 * and then rewinds the file to the beginning.
 * @param fptr Pointer to a FILE object that identifies the stream.
 * @return The length of the file in bytes.
 */
static size_t get_file_length(FILE* fptr) {
    assert(fptr);
    fseek(fptr, 0L, SEEK_END);
    size_t sz = ftell(fptr);
    rewind(fptr);
    return sz;
}

/**
 * @brief Reads the entire contents of a file into a string.
 *
 * Reads the file line by line into a buffer and appends each line to a hstr (high-performance string) object,
 * effectively concatenating the entire file content into a single string.
 * @param fptr Pointer to a FILE object that identifies the stream to be read.
 * @return A hstr containing the entire contents of the file.
 */
static hstr read_whole_file(FILE* fptr) {
    hstr file_str = hstr_new("");

    char buffer[100];
    while (fgets(buffer, 100, fptr)) {
        file_str = hstr_concat(file_str, hstr_new(buffer));
    }
    return file_str;
}

#endif