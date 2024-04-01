/*! \file ref_strings.c
 *  \brief reference counted strings
 *
 */

#include "ref_strings.h"
#include "allocator.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*! \struct hstrInternal
 * \brief Internal state for `hstr`
 */
typedef struct hstrInternal {

    /*! \brief Pointer to raw string data */
    char* raw;

    /*! \brief Number of references to this string */
    int references;

} hstrInternal;

/*! \struct hstrArrayInternal
 * \brief Internal state for `hstrArray`
 */
typedef struct hstrArrayInternal {

    /*! \brief number of items in array */
    int n_items;

    /*! \brief number of allocated items in array */
    int n_allocated;

    /*! \brief pointers to the `hstr`s */
    hstr* strings;

} hstrArrayInternal;

/*!
 * \brief Deletes a `hstr`, regardless of the number of references
 *
 * \param str The `hstr` to delete
 */
static void hstr_delete(hstr str) {
    LOG_FUNC_CALL();
    assert(str);
    assert(str->raw);
    if (str->references > 0) {
        LOG(WARNING, "freeing ref counted string with positive references (%d)",
            str->references);
    }
    FREE(str->raw);
    FREE(str);
}

/*!
 * \brief Constructs a `hstr` from a raw `const char*` with `0` references.
 *
 * \param input The raw `const char*` to construct the `hstr` from.
 * \return The allocated `hstr`.
 */
static hstr hstr_new_internal(const char* input) {
    LOG_FUNC_CALL();
    hstr out = MALLOC(sizeof(hstrInternal));
    out->raw = MALLOC(sizeof(char) * (strlen(input) + 2));
    strcpy(out->raw, input);
    out->references = 0;
    return out;
}

/*!
 * \brief Constructs a `hstr` from a raw `const char*` with `1` reference.
 *
 * This calls `hstr_new_internal` only, but maybe in the future
 * we will want to more stuff?
 *
 * \param input The raw `const char*` to construct the `hstr` from.
 * \return The allocated `hstr`.
 */
hstr hstr_new(const char* input) {
    LOG_FUNC_CALL();
    hstr out = hstr_new_internal(input);
    return out;
}

/*!
 * \brief Increments the references on a `hstr`.
 *
 * Will panic if the number of references is less than `0`.
 *
 * \param str The raw `hstr` to increment references on.
 * \return The original `hstr`, for convenience.
 */
hstr hstr_incref(hstr str) {
    LOG_FUNC_CALL();
    assert(str);
    assert(str->raw);
    PANIC_ON(str->references < 0,
             "incrementing references on string that has %d < 0 references",
             str->references);
    str->references++;
    return str;
}

/*!
 * \brief Decrements the references on a `hstr`.
 *
 * Will panic if the number of references is less than `1`.
 *
 * \param str The raw `hstr` to decrement references on.
 */
void hstr_decref(hstr str) {
    LOG_FUNC_CALL();
    assert(str);
    assert(str->raw);
    PANIC_ON(str->references < 1,
             "decrementing references on string that has %d < 1 references",
             str->references);
    str->references--;
    if (str->references == 0) {
        hstr_delete(str);
    }
}

/*!
 * \brief Unpacks the raw string from a `hstr`.
 *
 * Will panic if the number of references is less than `1`.
 *
 * \param str The raw `hstr` to unpack.
 */
const char* hstr_unpack(hstr str) {
    LOG_FUNC_CALL();
    assert(str);
    assert(str->raw);
    PANIC_ON(str->references < 1, "unpacking string that has %d < 1 references",
             str->references);
    return str->raw;
}

/*!
 * \brief Concatenates two `hstr`s.
 *
 *
 * \param left The first `hstr`.
 * \param right The second `hstr`.
 * \return The concatenated `hstr`.
 */
hstr hstr_concat(hstr left, hstr right) {
    LOG_FUNC_CALL();
    hstr_incref(left);
    hstr_incref(right);

    const char* lstr = hstr_unpack(left);
    const char* rstr = hstr_unpack(right);

    char out_str[(strlen(lstr) + strlen(rstr) + 2)];
    strcpy(out_str, lstr);
    strcat(out_str, rstr);

    hstr out = hstr_new(out_str);

    hstr_decref(left);
    hstr_decref(right);
    return out;
}

/*!
 * \brief Makes an empty `hstrArray`.
 *
 * \return An empty `hstrArray`.
 */
hstrArray hstr_array_make(void) {
    LOG_FUNC_CALL();
    hstrArray out = MALLOC(sizeof(hstrArrayInternal));
    out->n_items = 0;
    out->n_allocated = 2;
    out->strings = MALLOC(sizeof(hstr) * out->n_allocated);
    return out;
}

/*!
 * \brief Appends a `hstr` to a `hstrArray`.
 *
 * \param arr The `hstrArray` to append to.
 * \param str The `hstr` to append.
 */
void hstr_array_append(hstrArray arr, hstr str) {
    LOG_FUNC_CALL();
    assert(arr);
    assert(str);
    assert(arr->strings);
    assert(arr->n_allocated > 0);
    assert(arr->n_items >= 0);
    hstr_incref(str);

    if (arr->n_items >= arr->n_allocated) {
        arr->n_allocated *= 2;
        arr->strings = REALLOC(arr->strings, sizeof(hstr) * arr->n_allocated);
    }
    assert(arr->n_items < arr->n_allocated);
    arr->strings[arr->n_items] = str;
    arr->n_items++;
}

/*!
 * \brief Deletes (frees) a `hstrArray`.
 *
 * \param arr The `hstrArray` delete.
 */
void hstr_array_delete(hstrArray arr) {
    LOG_FUNC_CALL();
    assert(arr);
    assert(arr->strings);
    assert(arr->n_allocated > 0);
    assert(arr->n_items >= 0);
    for (int i = 0; i < arr->n_items; i++) {
        assert(arr->strings[i]);
        hstr_decref(arr->strings[i]);
    }
    FREE(arr->strings);
    FREE(arr);
}

/*!
 * \brief Splits a `hstr` into a `hstrArray`.
 *
 * \param str The `hstr` to split.
 * \param delim The `const char*` delimiter to split by.
 * \return The output `hstrArray`.
 */
hstrArray hstr_split(hstr str, const char* delim) {
    LOG_FUNC_CALL();
    assert(str);
    hstr_incref(str);

    hstrArray out = hstr_array_make();

    const char* raw_str = hstr_unpack(str);
    char mod_str[strlen(raw_str) + 2];
    strcpy(mod_str, raw_str);
    char* token = strtok(mod_str, delim);
    if (!token) {
        LOG(WARNING, "returning an empty array from hstr_split(%s,%s) (?)",
            raw_str, delim);
        return out;
    }
    while (token) {
        // printf("\nstrlen %s %d\n",token,strlen(token));
        hstr_array_append(out, hstr_new(token));//hstr_concat(hstr_new(token), hstr_new(delim)));
        token = strtok(NULL, delim);
    }

    hstr_decref(str);
    return out;
}

/*!
 * \brief Gets a `hstr` from a `hstrArray` and does bounds checking.
 *
 * \param arr The `hstrArray` to get from.
 * \param i The index to read.
 * \return The output `hstr`.
 */
hstr hstr_array_get(hstrArray arr, int i) {
    LOG_FUNC_CALL();
    assert(arr);
    assert(arr->strings);
    assert(arr->n_items > i);
    assert(i >= 0);
    return arr->strings[i];
}

/*!
 * \brief Gets the length of a `hstrArray`
 *
 * \param arr The `hstrArray` to get length of.
 * \return The length of the `hstrArray`.
 */
int hstr_array_len(hstrArray arr) {
    LOG_FUNC_CALL();
    assert(arr);
    assert(arr->strings);
    assert(arr->n_items >= 0);
    return arr->n_items;
}

/*!
 * \brief Checks if a string is NULL terminated
 *
 * \param str the string to check
 * \param sz the max size of the string
 * \return 1 if the string is NULL terminated, 0 if not.
 */
int hq_string_is_null_terminated(const char* str, int sz) {
    LOG_FUNC_CALL();
    for (int i = 0; i < sz; i++) {
        if (str[i] == '\0')
            return 1;
    }
    return 0;
}

static char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

hstr hstr_strip(hstr str){
    assert(str);
    hstr_incref(str);

    char raw[strlen(hstr_unpack(str)) + 2];
    strcpy(raw,hstr_unpack(str));
    char* stripped = trimwhitespace(raw);
    hstr out = hstr_new(stripped);

    hstr_decref(str);
    return out;
}