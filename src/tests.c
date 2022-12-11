#include "tests.h"
#include "mem.h"
#include "mem_internals.h"
#include "util.h"

#define HEAP_SIZE 4096
#define BLOCK_SIZE 256

static void print_heap(const void *heap, const char *string) {
    printf("%s:\n", string);
    debug_heap(stdout, heap);
}

static bool test_allocate_block() {
    void *heap = heap_init(HEAP_SIZE);
    if (!heap) {
        printf("test_allocate_block error!\n");
        return false;
    }

    print_heap(heap, "heap before test_allocate");

    void *allocated_memory = _malloc(BLOCK_SIZE);
    if (!allocated_memory) {
        printf("test_allocate_block error!\n");
        return false;
    }
    print_heap(heap, "heap after test_allocate_block");

    _free(allocated_memory);
    munmap(heap, size_from_capacity((block_capacity) {.bytes = HEAP_SIZE}).bytes);
    printf("test_allocate_block completed");
    return true;
}

static bool test_allocate_two_blocks() {
    void *heap = heap_init(HEAP_SIZE);
    if (!heap) {
        printf("test_allocate_two_blocks error!\n");
        return false;
    }

    print_heap(heap, "heap before test_allocate_two_blocks");

    void *first_block = _malloc(BLOCK_SIZE);
    void *second_block = _malloc(BLOCK_SIZE);

    if (!first_block) {
        if (!second_block) {
            printf("test_allocate_two_blocks: both bocks corrupted!\n");
        } else {
            printf("test_allocate_two_blocks: first bock corrupted!\n");
        }
        return false;
    } else if (!second_block) {
        printf("test_allocate_two_blocks: second bock corrupted!\n");
        return false;
    }


    print_heap(heap, "heap after test_allocate_two_blocks");

    _free(first_block);
    munmap(heap, size_from_capacity((block_capacity) {.bytes = HEAP_SIZE}).bytes);
    printf("test_allocate_two_blocks completed");
    return true;
}

bool tests_run() {
    bool ret = (test_allocate_block() && test_allocate_two_blocks());
    return ret;
}