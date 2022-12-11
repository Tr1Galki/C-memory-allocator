#include "tests.h"
#include "mem.h"
#include "mem_internals.h"
#include "util.h"

#define HEAP_SIZE  2048

static bool test_allocate() {
    void *heap = heap_init(REGION_MIN_SIZE);
    if (!heap) {
        return false;
    }
    int8_t *allocated_memory = _malloc(HEAP_SIZE);
    bool test_completed = false;
    if (allocated_memory) {
        // If no segmentation fault, ok
        for (size_t i = 0; i < HEAP_SIZE; i++) allocated_memory[i] = i;

        test_completed = true;
    }
    _free(allocated_memory);
    munmap(heap, region_actual_size(REGION_MIN_SIZE));
    return test_completed;
}

bool test_run() {
    return test_allocate();
}