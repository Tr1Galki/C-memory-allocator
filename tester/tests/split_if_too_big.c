#define TEST_SMART_MMAP

#include "test.h"

#include <assert.h>

// test when block is not free
DEFINE_TEST(not_free) {
    uint8_t buffer[128] = { 0 };

    block_init(buffer, (block_size) { .bytes = 128 }, NULL);

    struct block_header * const block = (void*) buffer;
    block->is_free = false;

    assert(!split_if_too_big(block, 120 - BLOCK_MIN_CAPACITY - offsetof(struct block_header, contents)));
    assert(block->next == NULL);
    assert(block->capacity.bytes == 128 - offsetof(struct block_header, contents));
    assert(block->is_free == false);
}

// test when block is not big enough
DEFINE_TEST(not_big_enough) {
    uint8_t buffer[128] = { 0 };

    block_init(buffer, (block_size) { .bytes = 128 }, NULL);

    struct block_header * const block = (void*) buffer;

    assert(!split_if_too_big(block, 120));
    assert(block->next == NULL);
    assert(block->capacity.bytes == 128 - offsetof(struct block_header, contents));
    assert(block->is_free == true);
}

// test when block is not big enough for min capacity
DEFINE_TEST(not_big_enough_for_min_capacity) {
    uint8_t buffer[128] = { 0 };

    block_init(buffer, (block_size) { .bytes = 128 }, NULL);

    struct block_header * const block = (void*) buffer;

    assert(!split_if_too_big(block, 120 - offsetof(struct block_header, contents)));
    assert(block->next == NULL);
    assert(block->capacity.bytes == 128 - offsetof(struct block_header, contents));
    assert(block->is_free == true);
}

// test successful split
DEFINE_TEST(successful) {
    uint8_t buffer[128] = { 0 };

    block_init(buffer, (block_size) { .bytes = 128 }, NULL);

    struct block_header * const block = (void*) buffer;

    assert(split_if_too_big(block, 24));

    struct block_header * const next_block = block->next;

    assert(next_block != NULL);
    assert(block->capacity.bytes == 24);
    assert(block->is_free == true);

    assert(next_block->next == NULL);
    assert(next_block->capacity.bytes == 128 - 2 * offsetof(struct block_header, contents) - 24);
    assert(next_block->is_free == true);
}

DEFINE_TEST_GROUP(not_splittable) {
    TEST_IN_GROUP(not_free),
    TEST_IN_GROUP(not_big_enough),
    TEST_IN_GROUP(not_big_enough_for_min_capacity),
};

int main() {
    RUN_TEST_GROUP(not_splittable);
    RUN_SINGLE_TEST(successful);
    return 0;
}
