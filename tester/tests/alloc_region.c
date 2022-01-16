#define TEST_SMART_MMAP

#include "test.h"

#include <assert.h>


DEFINE_MMAP_IMPL(test_query_is_zero) {
    base_mmap_checks(addr, length, prot, flags, fd, offset);
    assert(length == REGION_MIN_SIZE);
    return MAP_FAILED;
}

DEFINE_TEST(query_is_zero) {
    current_mmap_impl = MMAP_IMPL(test_query_is_zero);
    alloc_region(0, 0);
}

DEFINE_TEST(query_is_small) {
    current_mmap_impl = MMAP_IMPL(test_query_is_zero);
    alloc_region(0, 42);
}

DEFINE_TEST_GROUP(query) {
    TEST_IN_GROUP(query_is_zero),
    TEST_IN_GROUP(query_is_small),
};

int main() {
    RUN_TEST_GROUP(query);
    return 0;
}
