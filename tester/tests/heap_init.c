#define TEST_SMART_MMAP

#include "test.h"

#include <time.h>


static size_t test_length = 0;
static int mmap_counter = 0;

DEFINE_MMAP_IMPL(fail) {
    base_mmap_checks(addr, length, prot, flags, fd, offset);
    assert(addr == HEAP_START && length == test_length);
    ++mmap_counter;
    return MAP_FAILED;
}

// test when mmap failed
DEFINE_TEST(fail) {
    current_mmap_impl = MMAP_IMPL(success);
    srand(time(NULL));

    for (int i = 0; i < 10; ++i) {
        test_length = rand();
        mmap_counter = 0;

        printf("heap_init(%zu)\n", test_length);

        assert(heap_init(test_length) == NULL);
        assert(mmap_counter == 2);
    }
}

static void * mmap_result = NULL;

DEFINE_MMAP_IMPL(success) {
    base_mmap_checks(addr, length, prot, flags, fd, offset);
    assert(addr == HEAP_START && length == test_length);
    ++mmap_counter;
    return (mmap_result = mmap(addr, length, prot, flags, fd, offset));
}

// test when mmap success
DEFINE_TEST(success) {
    current_mmap_impl = MMAP_IMPL(success);
    srand(time(NULL));

    for (int i = 0; i < 10; ++i) {
        test_length = rand();
        mmap_counter = 0;

        printf("heap_init(%zu)\n", test_length);

        assert(heap_init(test_length) == mmap_result);
        assert(mmap_counter == 2);
    }
}

int main() {
    RUN_SINGLE_TEST(fail);
    RUN_SINGLE_TEST(success);
    return 0;
}
