#include "test.h"

#include <string.h>

static int test_optimistic_case_call_counter = 0;
DEFINE_MAP_PAGES_IMPL(optimistic_case) {
    ++test_optimistic_case_call_counter;

    if (test_optimistic_case_call_counter == 1) {
        assert(location == PAGE_FIXED);
    } else if (test_optimistic_case_call_counter == 2) {
        assert(location == PAGE_ANYWHERE);
    } else {
        assert(false);
    }

    return platform_map_pages(addr, length, location);
}

// test with real HEAP_START and successful mmap MAP_FIXED
DEFINE_TEST(optimistic_case) {
    USE_MAP_PAGES_IMPL(optimistic_case);

    const struct region region = alloc_region(HEAP_START, 0);

    assert(region.addr == HEAP_START);
    assert(region.size == REGION_MIN_SIZE);
    assert(region.extends);

    assert(test_optimistic_case_call_counter == 1);

    struct block_header * block = region.addr;
    assert(block->next == NULL);
    assert(block->capacity.bytes == REGION_MIN_SIZE - offsetof(struct block_header, contents));
    assert(block->is_free);

    memset(block->contents, 42, block->capacity.bytes);

    unmap_pages(region.addr, region.size);
}

static int test_map_fixed_failed_mmap_counter = 0;
DEFINE_MAP_PAGES_IMPL(map_fixed_failed) {
    ++test_map_fixed_failed_mmap_counter;
    if (test_map_fixed_failed_mmap_counter == 1) {
        return MAP_PAGES_FAILURE;
    }

    return platform_map_pages(NULL, length, location);
}

// test with real HEAP_START and failing mmap MAP_FIXED
DEFINE_TEST(map_fixed_failed) {
    USE_MAP_PAGES_IMPL(map_fixed_failed);

    const struct region region = alloc_region(HEAP_START, 0);

    assert(region.addr != HEAP_START);
    assert(region.size == REGION_MIN_SIZE);
    assert(!region.extends);

    assert(test_map_fixed_failed_mmap_counter == 2);

    struct block_header * block = region.addr;
    assert(block->next == NULL);
    assert(block->capacity.bytes == REGION_MIN_SIZE - offsetof(struct block_header, contents));
    assert(block->is_free);

    memset(block->contents, 42, block->capacity.bytes);

    unmap_pages(region.addr, region.size);
}

static int test_pessimistic_case_mmap_counter = 0;
DEFINE_MAP_PAGES_IMPL(pessimistic_case) {
    ++test_pessimistic_case_mmap_counter;
    return MAP_PAGES_FAILURE;
}

// test with real HEAP_START and failing mmap MAP_FIXED
DEFINE_TEST(pessimistic_case) {
    USE_MAP_PAGES_IMPL(pessimistic_case);

    const struct region region = alloc_region(HEAP_START, 0);

    assert(region.addr == NULL);
    assert(test_pessimistic_case_mmap_counter == 2);
}

DEFINE_MAP_PAGES_IMPL(query_is_small) {
    assert(length == REGION_MIN_SIZE);
    return MAP_PAGES_FAILURE;
}

// test with query = 0
DEFINE_TEST(query_is_zero) {
    USE_MAP_PAGES_IMPL(query_is_small);
    alloc_region(HEAP_START, 0);
}

// test with query > 0 and query < REGION_MIN_SIZE
DEFINE_TEST(query_is_small) {
    USE_MAP_PAGES_IMPL(query_is_small);
    alloc_region(HEAP_START, 42);
}

DEFINE_MAP_PAGES_IMPL(query_is_min_region_size) {
    assert(length == (size_t) (REGION_MIN_SIZE + page_size()));
    return platform_map_pages(addr, length, location);
}

// test with query == REGION_MIN_SIZE (to check is block header size counted in mmap length)
DEFINE_TEST(query_is_min_region_size) {
    USE_MAP_PAGES_IMPL(query_is_min_region_size);

    const struct region region = alloc_region(HEAP_START, REGION_MIN_SIZE);

    assert(region.addr == HEAP_START);
    assert(region.size == (size_t) (REGION_MIN_SIZE + page_size()));
    assert(region.extends);

    struct block_header * block = region.addr;
    assert(block->next == NULL);
    assert(block->capacity.bytes == region.size - offsetof(struct block_header, contents));
    assert(block->is_free);

    memset(block->contents, 42, block->capacity.bytes);

    unmap_pages(region.addr, region.size);
}

DEFINE_TEST_GROUP(query) {
    TEST_IN_GROUP(query_is_zero),
    TEST_IN_GROUP(query_is_small),
    TEST_IN_GROUP(query_is_min_region_size),
};

int main() {
    RUN_SINGLE_TEST(optimistic_case);
    RUN_SINGLE_TEST(map_fixed_failed);
    RUN_SINGLE_TEST(pessimistic_case);
    RUN_TEST_GROUP(query);
    return 0;
}
