/*! \file ref_strings.h
 *  \brief ref_strings.c headers
 *
 */

#ifndef _HQ_REF_STRINGS_H_
#define _HQ_REF_STRINGS_H_

struct hstrInternal;
typedef struct hstrInternal* hstr;

struct hstrArrayInternal;
typedef struct hstrArrayInternal* hstrArray;

hstr hstr_new(const char* input);
hstr hstr_incref(hstr str);
void hstr_decref(hstr str);
const char* hstr_unpack(hstr str);
hstr hstr_concat(hstr left, hstr right);

hstrArray hstr_array_make(void);
void hstr_array_append(hstrArray arr, hstr str);
void hstr_array_delete(hstrArray arr);

hstrArray hstr_split(hstr str, const char* delim);
hstr hstr_array_get(hstrArray arr, int i);
int hstr_array_len(hstrArray arr);

int hq_string_is_null_terminated(const char* str, int sz);
hstr hstr_strip(hstr str);
#endif
