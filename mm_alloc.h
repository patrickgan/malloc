/*
 * mm_alloc.h
 *
 * A clone of the interface documented in "man 3 malloc".
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>

/* Metadata structure. */
struct metadata {
    struct metadata *prev;
    struct metadata *next;
    bool free;
    size_t size;
};

void *mm_malloc(size_t size);
void *mm_realloc(void *ptr, size_t size);
void mm_free(void *ptr);
