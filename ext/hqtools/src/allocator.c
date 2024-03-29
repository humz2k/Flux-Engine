/*! \file allocator.c
 *  \brief a simple allocator to make things easy to debug
 *
 */

#include <stdio.h>
#include <stdlib.h>
#define HQTOOLS_DONT_REPLACE_MALLOC
#include "allocator.h"
#include "log.h"

/*! \struct allocation
 * \brief A single `allocation`.
 */
typedef struct allocation {

    /*! \brief Whether this allocation is "alive" (freed or not) */
    int alive;

    /*! \brief The pointer to this allocation */
    void* ptr;

    /*! \brief Size of this allocation */
    size_t sz;

    /*! \brief Line on which this pointer was allocated */
    int line;

    /*! \brief File in which this pointer was allocated */
    const char* file;

} allocation;

/*! \struct hqAllocatorInternal
 * \brief Internal state of `hqAllocator`
 */
typedef struct hqAllocatorInternal {

    /*! \brief The name of this allocator (for debugging)
     * This should always be a literal string NOT(!!!) something
     * dynamically allocated.
     */
    const char* name;

    /*! \brief Number of alive `allocation`s */
    int n_alive;

    /*! \brief Number of total `allocation`s (alive or otherwise) */
    int n_total;

    /*! \brief Number of total `allocation`s allocated for this `hqAllocator`
     * (i.e. dynamic array stuff) */
    int n_preallocated;

    /*! \brief Pointer to `struct allocation`s */
    allocation* allocations;

} hqAllocatorInternal;

/*!
 * \brief Initializes a `hqAllocator`
 *
 * This function initializes a `hqAllocator`.
 * It sets `n_alive` to 0, `n_total` to 0, and
 * sets `allocations` to `NULL` (this is managed
 * dynamically).
 * NOTE: `name` should be a LITERAL string, NOT
 * dynamically allocated.
 *
 * \param allocator The pointer to the `hqAllocator`.
 * \param name The name of this allocator - a LITERAL string.
 */
void hq_allocator_init(hqAllocator* allocator, const char* name) {
    //LOG_FUNC_CALL();
    //LOG(DEBUG, "initializing allocator %s", name);
    assert(*allocator =
               (hqAllocator)malloc(sizeof(struct hqAllocatorInternal)));
    (*allocator)->n_alive = 0;
    (*allocator)->n_total = 0;
    (*allocator)->n_preallocated = 10;
    assert((*allocator)->allocations = (allocation*)malloc(
               sizeof(allocation) * (*allocator)->n_preallocated));
    (*allocator)->name = name;
    //LOG(DEBUG, "initialized allocator %s", name);
}

/*!
 * \brief Deletes a `hqAllocator`
 *
 * This function deletes a `hqAllocator`.
 *
 * \param allocator The `hqAllocator` to delete.
 */
void hq_allocator_delete(hqAllocator allocator) {
    //LOG_FUNC_CALL();

    assert(allocator);
    assert(allocator->allocations);

    printf("deleting allocator %s\n", allocator->name);
    int n_alive = 0;
    for (int i = 0; i < allocator->n_total; i++) {
        allocation* this_allocation = &allocator->allocations[i];
        assert(this_allocation);
        if (this_allocation->alive) {
            if(this_allocation->ptr == NULL)
                     printf("allocator thinks ptr (%s:%d) is alive but it isn't\n",
                     this_allocation->file, this_allocation->line);
            printf("unfreed pointer %p<%lu bytes> (from %s:%d)\n",
                this_allocation->ptr, this_allocation->sz,
                this_allocation->file, this_allocation->line);
            free(this_allocation->ptr);
            n_alive++;
        } else {
            if (this_allocation->ptr != NULL)
                     printf("allocator thinks ptr (%s:%d) is dead but it isn't\n",
                     this_allocation->file, this_allocation->line);
        }
    }
    if(allocator->n_alive != n_alive)
             printf("allocator got number of alive pointers wrong\n");
    free(allocator->allocations);
    printf("deleted allocator %s (n_total = %d, n_alive = %d)\n",
        allocator->name, allocator->n_total, allocator->n_alive);
    free(allocator);
}

/*!
 * \brief Doubles number of preallocations in a `hqAllocator`.
 *
 * \param allocator The `hqAllocator` to double in size.
 */
static void grow_allocations(hqAllocator allocator) {
    LOG_FUNC_CALL();

    assert(allocator);

    // if these fail, there is a bug in this file somewhere
    assert(allocator->allocations);
    assert(allocator->n_preallocated > 0);

    allocator->n_preallocated *= 2;
    allocator->allocations = (allocation*)realloc(
        allocator->allocations, sizeof(allocation) * allocator->n_preallocated);
}

/*!
 * \brief Gets a fresh `allocation*` from `allocator`.
 *
 * \param allocator The `hqAllocator` to get a fresh `allocation*` from.
 * \return A fresh `allocation*`
 */
static allocation* get_fresh_allocation(hqAllocator allocator) {
    LOG_FUNC_CALL();

    assert(allocator);

    // if these fail, there is a bug in this file somewhere
    assert(allocator->allocations);
    assert(allocator->n_preallocated > 0);

    if (allocator->n_total >= allocator->n_preallocated)
        grow_allocations(allocator);

    assert(allocator->n_total < allocator->n_preallocated);

    allocation* out = &allocator->allocations[allocator->n_total];
    allocator->n_total++;

    return out;
}

/*!
 * \brief Allocates a pointer with size `sz` from `allocator`
 *
 * \param allocator The `allocator` to allocate from.
 * \param sz The size of the allocation.
 * \param file The file this allocation came from.
 * \param line The line this allocation came from.
 * \return The allocated pointer.
 */
void* hq_allocator_alloc(hqAllocator allocator, size_t sz, const char* file,
                         int line) {
    LOG_FUNC_CALL();

    assert(allocator);

    LOG(DEBUG,"allocating %lu bytes (%s:%d)",sz,file,line);

    allocation* this_allocation = get_fresh_allocation(allocator);
    assert(this_allocation->ptr = malloc(sz));
    this_allocation->alive = 1;
    this_allocation->file = file;
    this_allocation->line = line;
    this_allocation->sz = sz;

    allocator->n_alive++;

    return this_allocation->ptr;
}

/*!
 * \brief Reallocates a pointer with size `sz` from `allocator`
 *
 * \param allocator The `allocator` to allocate from.
 * \param ptr The pointer to reallocate.
 * \param sz The size of the allocation.
 * \param file The file this allocation came from.
 * \param line The line this allocation came from.
 * \return The allocated pointer.
 */
void* hq_allocator_realloc(hqAllocator allocator, void* ptr, size_t sz,
                           const char* file, int line) {
    LOG_FUNC_CALL();
    assert(allocator);

    if (ptr == NULL) {
        LOG(ERROR, "tried to realloc a NULL pointer (%s:%d)", file, line);
        return hq_allocator_alloc(allocator,sz,file,line);
        //exit(1);
    }

    for (int i = 0; i < allocator->n_total; i++) {
        allocation* this_allocation = &allocator->allocations[i];
        if (ptr == this_allocation->ptr) {
            assert(this_allocation->alive);
            LOG(DEBUG,
                "reallocating pointer %p (originated in %s:%d, reallocated in "
                "%s:%d)",
                ptr, this_allocation->file, this_allocation->line, file, line);
            this_allocation->sz = sz;
            this_allocation->ptr =
                realloc(this_allocation->ptr, this_allocation->sz);
            return this_allocation->ptr;
        }
    }

    LOG(ERROR,
        "tried to reallocate pointer %p (%s:%d) in allocator %s but it doesn't "
        "exist",
        ptr, file, line, allocator->name);
    exit(1);
    return NULL;
}

/*!
 * \brief Frees pointer `ptr` from `allocator`
 *
 * \param allocator The `allocator` to free from.
 * \param ptr The pointer to free.
 * \param file The file this free came from.
 * \param line The line this free came from.
 */
void hq_allocator_free(hqAllocator allocator, void* ptr, const char* file,
                       int line) {
    LOG_FUNC_CALL();

    assert(allocator);
    assert(allocator->allocations);

    if (ptr == NULL) {
        LOG(ERROR, "tried to free a NULL pointer (%s:%d)", file, line);
        return;
    }

    for (int i = 0; i < allocator->n_total; i++) {
        allocation* this_allocation = &allocator->allocations[i];
        if (ptr == this_allocation->ptr) {
            assert(this_allocation->alive);
            LOG(DEBUG,
                "freeing pointer %p (originated in %s:%d, freed in %s:%d)", ptr,
                this_allocation->file, this_allocation->line, file, line);
            free(ptr);
            this_allocation->ptr = NULL;
            this_allocation->alive = 0;

            allocator->n_alive--;
            assert(allocator->n_alive >= 0);
            LOG(DEBUG, "success");
            return;
        }
    }

    LOG(ERROR,
        "tried to free pointer %p (%s:%d) in allocator %s but it doesn't exist",
        ptr, file, line, allocator->name);
    free(ptr);
}

hqAllocator hq_global_allocator;

/*!
 * \brief Initializes the `hq_global_allocator`
 *
 * This function initializes `hq_global_allocator`, which is what
 * the `MALLOC` and `FREE` macros use.
 * This should be called ONCE before anything else.
 */
void hq_allocator_init_global(void) {
    //LOG_FUNC_CALL();
    hq_allocator_init(&hq_global_allocator, "hq_global_allocator");
}

/*!
 * \brief Deletes the `hq_global_allocator`
 *
 * This function deletes `hq_global_allocator`, which is what
 * the `MALLOC` and `FREE` macros use.
 * This should be called ONCE after everything else.
 */
void hq_allocator_delete_global(void) {
    //LOG_FUNC_CALL();
    hq_allocator_delete(hq_global_allocator);
}