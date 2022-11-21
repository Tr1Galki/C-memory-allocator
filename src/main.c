#define _DEFAULT_SOURCE

#include <stdio.h>

#include "mem.h"
#include "mem_internals.h"

int main() {
    heap_init(REGION_MIN_SIZE);

    uint8_t* block = _malloc(64);
    _free(block);
    printf("Simple malloc test PASSED\n");

    uint8_t* blocks[5] = {NULL};
    for (size_t i = 0; i < 5; i++) {
        blocks[i] = _malloc(64);
    }
    _free(blocks[1]);
    printf("Single free test PASSED\n");

    _free(blocks[3]);
    _free(blocks[4]);
    printf("Multiple free test PASSED\n");
    _free(blocks[0]);
    _free(blocks[2]);

    block = _malloc(4 * REGION_MIN_SIZE);
    printf("Region extend test PASSED\n");
    _free(block);
    return 0;
}
