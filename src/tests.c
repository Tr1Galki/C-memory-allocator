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

static void memory_free(const void *heap, void *allocated_memory) {
    _free(allocated_memory);
    munmap(heap, size_from_capacity((block_capacity) {.bytes = HEAP_SIZE}).bytes);
}

static bool test_allocate_block() {
    void *heap = heap_init(HEAP_SIZE);
    if (!heap) {
        printf("test_allocate_block error!\n");
        return false;
    }

    print_heap(heap, "heap before test_allocate_block");

    void *allocated_memory = _malloc(BLOCK_SIZE);

    print_heap(heap, "heap after test_allocate_block");

    if (!allocated_memory) {
        printf("test_allocate_block error!\n");
        return false;
    }

    memory_free(heap, allocated_memory);
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

    print_heap(heap, "heap after test_allocate_two_blocks");

    if (!first_block) {
        if (!second_block) {
            printf("test_allocate_two_blocks: both blocks corrupted!\n");
        } else {
            printf("test_allocate_two_blocks: first block corrupted!\n");
        }
        return false;
    } else if (!second_block) {
        printf("test_allocate_two_blocks: second block corrupted!\n");
        return false;
    }

    memory_free(heap, first_block);
    printf("test_allocate_two_blocks completed");
    return true;
}

static bool test_allocate_three_different_blocks() {
    void *heap = heap_init(HEAP_SIZE);
    if (!heap) {
        printf("test_allocate_three_different_blocks error!\n");
        return false;
    }

    print_heap(heap, "heap before test_allocate_three_different_blocks");

    void *first_block = _malloc(BLOCK_SIZE/2);
    void *second_block = _malloc(BLOCK_SIZE*2);
    void *third_block = _malloc(BLOCK_SIZE);

    print_heap(heap, "heap after test_allocate_three_different_blocks");

    if (!first_block) {
        if (!second_block) {
            if (!third_block) {
                printf("test_allocate_three_different_blocks: all blocks corrupted!\n");
            } else {
                printf("test_allocate_three_different_blocks: first and second blocks corrupted!\n");
            }
        } else {
            if (!third_block) {
                printf("test_allocate_three_different_blocks: first and third blocks corrupted!\n");
            } else {
                printf("test_allocate_three_different_blocks: first block corrupted!\n");
            }
        }
        return false;
    } else if (!second_block) {
        if (!third_block) {
            printf("test_allocate_three_different_blocks: second and third blocks corrupted!\n");
        } else {
            printf("test_allocate_three_different_blocks: second block corrupted!\n");
        }
        return false;
    } else if (!third_block) {
        printf("test_allocate_three_different_blocks: third block corrupted!\n");
        return false;
    }
    
    memory_free(heap, first_block);
    printf("test_allocate_two_blocks completed");
    return true;
}

bool tests_run() {
    bool ret = (test_allocate_block() && test_allocate_two_blocks() && test_allocate_three_different_blocks());
    return ret;
}
