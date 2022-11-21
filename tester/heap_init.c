#define TEST_SMART_MMAP

#include "test.h"


static const size_t corner_cases[] = { 0, 10, 24, 24 + offsetof(struct block_header, contents),
    REGION_MIN_SIZE - offsetof(struct block_header, contents), REGION_MIN_SIZE };

static size_t test_length = 0;
static int mmap_counter = 0;

DEFINE_MAP_PAGES_IMPL(fail) {
    assert(addr == HEAP_START && length == region_actual_size(test_length + offsetof(struct block_header, contents)));
    ++mmap_counter;
    return MAP_PAGES_FAILURE;
}

// test when mmap failed
DEFINE_TEST(fail) {
    USE_MAP_PAGES_IMPL(fail);

    for (size_t i = 0; i < sizeof(corner_cases) / sizeof(*corner_cases); ++i) {
        test_length = corner_cases[i];
        mmap_counter = 0;

        fprintf(stderr, "heap_init(%zu)\n", test_length);

        assert(heap_init(test_length) == NULL);
        assert(mmap_counter == 2);
    }
}

static void * mmap_result = NULL;

DEFINE_MAP_PAGES_IMPL(success) {
    assert(addr == HEAP_START && length == region_actual_size(test_length + offsetof(struct block_header, contents)));
    ++mmap_counter;
    return (mmap_result = platform_map_pages(addr, length, location));
}

// test when mmap success
DEFINE_TEST(success) {
    USE_MAP_PAGES_IMPL(success);

    for (size_t i = 0; i < sizeof(corner_cases) / sizeof(*corner_cases); ++i) {
        test_length = corner_cases[i];
        mmap_counter = 0;

        fprintf(stderr, "heap_init(%zu)\n", test_length);

        void * const result = heap_init(test_length);

        assert(result == mmap_result);
        assert(mmap_counter == 1);

        unmap_pages(result, region_actual_size(test_length + offsetof(struct block_header, contents)));
    }
}

int main() {
    RUN_SINGLE_TEST(fail);
    RUN_SINGLE_TEST(success);
    return 0;
}
