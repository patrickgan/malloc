/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines.
 */

#include "mm_alloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

void *heap_base = NULL;
void *prev_base = NULL;

/* Allocates heap memory. Checks if there is a sufficiently large free
   block and returns pointer if found. Else, increases the heap
   boundary and returns a new block.

   Uses a metadata structure containing pointers to the previous and next
   blocks, the size of the block, and whether the block is free. */
void *mm_malloc(size_t size) {
    if (size == 0)
        return NULL;

    if (heap_base == NULL)
        heap_base = sbrk(0);

    struct metadata *md = (struct metadata *) heap_base;
    while ((void*) md < sbrk(0)) {
        if (md->free) {
            if (md->size >= size) {
                md->free = false;
                if (md->size > size + sizeof(struct metadata)) {
                    struct metadata new_md = {md, md->next, true, md->size - size - sizeof(struct metadata)};
                    struct metadata *md2 = (struct metadata *) ((void *) md + sizeof(struct metadata) + size);
                    *md2 = new_md;
                    md->next = md2;
                }
                void *data = (void *) md + sizeof(struct metadata);
                memset(data, 0, size);
                return data;
            }
        }
        md = md->next;
    }

    void *check = sbrk(sizeof(struct metadata) + size);
    if (check == (void *) -1)
        return NULL;

    struct metadata new_md = {prev_base, sbrk(0), false, size};
    *md = new_md;
    prev_base = md;
    void *data = (void *) md + sizeof(struct metadata);
    memset(data, 0, size);
    return data;
}

/* Reallocates memory to a new size, preserving data accordingly. */
void *mm_realloc(void *ptr, size_t size) {
    if (ptr == NULL)
        return mm_malloc(size);

    struct metadata *md = (struct metadata *) ((void *) ptr - sizeof(struct metadata));

    char buf[md->size < size ? md->size : size];
    memcpy(buf, ptr, md->size < size ? md->size : size);

    mm_free(ptr);
    
    if (size == 0) {
        return NULL;
    } else {
        void *ptr2 = mm_malloc(size);
        if (ptr2) {
            memcpy(ptr2, buf, md->size < size ? md->size : size);
            return ptr2;
        } else {
            return NULL;
        }
    }
}

/* Frees a block of memory. If the following block is also free, coalesces
   the two free blocks into one larger free block. */
void mm_free(void *ptr) {
    if (ptr == NULL)
        return;

    struct metadata *md = (struct metadata *) ((void *) ptr - sizeof(struct metadata));
    md->free = true;
    if (md->next->free) {
        md->size += sizeof(struct metadata) + md->next->size;
        md->next = md->next->next;
    }
    return;
}
