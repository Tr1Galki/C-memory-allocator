#include "page_routines.h"

#include <sys/mman.h>
#include <unistd.h>

void* platform_map_pages(void const* addr, size_t length, enum page_location location) {
    int additional_flags = (location == PAGE_FIXED) ? MAP_FIXED_NOREPLACE : 0;
    void* mapping = mmap( (void*) addr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | additional_flags, -1, 0 );
    return (mapping == MAP_FAILED) ? MAP_PAGES_FAILURE : mapping;
}

int platform_unmap_pages(void const* addr, size_t length) {
    return munmap(addr, length);
}

int platform_page_size() {
    return getpagesize();
}
