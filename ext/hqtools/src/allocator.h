/*! \file allocator.h
 *  \brief allocator.c headers
 *
 */

#ifndef _HQ_ALLOCATOR_H_
#define _HQ_ALLOCATOR_H_

#include <stdlib.h>

struct hqAllocatorInternal;
typedef struct hqAllocatorInternal* hqAllocator;

extern hqAllocator hq_global_allocator;

void hq_allocator_init(hqAllocator* allocator, const char* name);
void hq_allocator_delete(hqAllocator allocator);

void* hq_allocator_alloc(hqAllocator allocator, size_t sz, const char* file,
                         int line);
void* hq_allocator_realloc(hqAllocator allocator, void* ptr, size_t sz,
                           const char* file, int line);
void hq_allocator_free(hqAllocator allocator, void* ptr, const char* file,
                       int line);

void hq_allocator_init_global(void);
void hq_allocator_delete_global(void);

#include "log.h"

#define HQ_ALLOC(allocator, sz)                                                \
    hq_allocator_alloc(allocator, sz, __FILENAME__, __LINE__)
#define HQ_REALLOC(allocator, ptr, sz)                                         \
    hq_allocator_realloc(allocator, ptr, sz, __FILENAME__, __LINE__)
#define HQ_FREE(allocator, ptr)                                                \
    hq_allocator_free(allocator, ptr, __FILENAME__, __LINE__)
#define MALLOC(sz) HQ_ALLOC(hq_global_allocator, sz)
#define REALLOC(ptr, sz) HQ_REALLOC(hq_global_allocator, ptr, sz)
#define FREE(ptr) HQ_FREE(hq_global_allocator, ptr)

#ifndef HQTOOLS_DONT_REPLACE_MALLOC
#define malloc(sz) MALLOC(sz)
#define free(sz) FREE(sz)
#define realloc(ptr, sz) REALLOC(ptr, sz)
#endif

#endif
