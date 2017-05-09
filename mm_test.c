#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

/* Function pointers to mm functions */
void* (*mm_malloc)(size_t);
void* (*mm_realloc)(void*, size_t);
void (*mm_free)(void*);

void load_alloc_functions() {
    void *handle = dlopen("mm_lib.so", RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    char* error;
    mm_malloc = dlsym(handle, "mm_malloc");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_realloc = dlsym(handle, "mm_realloc");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_free = dlsym(handle, "mm_free");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }
}

int main() {
    load_alloc_functions();

    printf("allocating block1 of size %lu\n", 0x200 * sizeof(int));
    int *block1 = (int*) mm_malloc(0x200 * sizeof(int));
    assert(block1 != NULL);
    printf("block1: 0x%lx\n", (unsigned long) block1);
    block1[0] = 0x162;
    
    printf("allocating block2 of size %lu\n", 0x200 * sizeof(int));
    int *block2 = (int*) mm_malloc(0x200 * sizeof(int));
    assert(block2 != NULL);
    printf("block2: 0x%lx\n", (unsigned long) block2);

    printf("reallocate block1 to size %lu\n", 0x400 * sizeof(int));
    block1 = mm_realloc(block1, 0x400 * sizeof(int));
    printf("block1: 0x%lx\n", (unsigned long) block1);

    printf("allocate block3 of size %lu\n", 0x200 * sizeof(int));
    int *block3 = (int*) mm_malloc(0x200 * sizeof(int));
    assert(block3 != NULL);
    printf("block3: 0x%lx\n", (unsigned long) block3);

    printf("reallocate block1 to size %lu\n", (unsigned long) 1 << 30);
    block1 = mm_realloc(block1, (unsigned long) 1 << 30);
    printf("block1: 0x%lx\n", (unsigned long) block1);

    printf("free block1\n");
    mm_free(block1);
    printf("free block2\n");
    mm_free(block2);
    printf("free block3\n");
    mm_free(block3);

    printf("allocating block of size %lu\n", 0x4 * sizeof(int));
    int *newblock1 = (int *) mm_malloc(0x4 * sizeof(int));
    assert(newblock1 != NULL);
    printf("newblock1: 0x%lx\n", (unsigned long) newblock1);
    printf("allocating block of size %lu\n", 0x4 * sizeof(int));
    int *newblock2 = (int *) mm_malloc(0x4 * sizeof(int));
    assert(newblock2 != NULL);
    printf("newblock2: 0x%lx\n", (unsigned long) newblock2);
    printf("free newblock2\n");
    mm_free(newblock2);

    printf("reallocating newblock1 to size %lu\n", 0x300 * sizeof(int));
    newblock1 = mm_realloc(newblock1, 0x300);
    assert(newblock1 != NULL);
    printf("newblock1: 0x%lx\n", (unsigned long) newblock1);
    printf("free newblock1\n");
    mm_free(newblock1);
    
    printf("malloc test successful!\n");
    return 0;
}
