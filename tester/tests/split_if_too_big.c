#define TEST_SMART_MMAP

#include "test.h"

#include <assert.h>

// test when block is not free
DEFINE_TEST(not_free) {
    uint8_t buffer[64] = { 0 };

    block_init(buffer, (block_size) { .bytes = 64 }, NULL);

    struct block_header * const block = (void*) buffer;
    block->is_free = false;

    assert(!split_if_too_big(block, 60 - BLOCK_MIN_CAPACITY - offsetof(struct block_header, contents)));
    assert(block->next == NULL);
    assert(block->capacity.bytes == 64 - offsetof(struct block_header, contents));
    assert(block->is_free == false);
}

// test when block is not big enough
DEFINE_TEST(not_big_enough) {
    uint8_t buffer[64] = { 0 };

    block_init(buffer, (block_size) { .bytes = 64 }, NULL);

    struct block_header * const block = (void*) buffer;

    assert(!split_if_too_big(block, 60));
    assert(block->next == NULL);
    assert(block->capacity.bytes == 64 - offsetof(struct block_header, contents));
    assert(block->is_free == true);
}

// test when block is not big enough for min capacity
DEFINE_TEST(not_big_enough_for_min_capacity) {
    uint8_t buffer[64] = { 0 };

    block_init(buffer, (block_size) { .bytes = 64 }, NULL);

    struct block_header * const block = (void*) buffer;

    assert(!split_if_too_big(block, 60 - offsetof(struct block_header, contents)));
    assert(block->next == NULL);
    assert(block->capacity.bytes == 64 - offsetof(struct block_header, contents));
    assert(block->is_free == true);
}

DEFINE_TEST_GROUP(not_splittable) {
    TEST_IN_GROUP(not_free),
    TEST_IN_GROUP(not_big_enough),
    TEST_IN_GROUP(not_big_enough_for_min_capacity),
};

int main() {
    RUN_TEST_GROUP(not_splittable);
    return 0;
}
