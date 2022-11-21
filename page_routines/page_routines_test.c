#include "page_routines_test.h"

#include <stdio.h>

static void log_map_pages_call(FILE * output, void const* addr, size_t length, enum page_location location, void* result) {
    fputs("addr = ", output);
    if (addr) {
        fprintf(output, "%p", addr);
    } else {
        fputs("NULL", output);
    }

    fprintf(output, ", length = %zu, ", length);

    if (location == PAGE_FIXED) {
        fputs("location = PAGE_FIXED) -> ", output);
    } else if (location == PAGE_ANYWHERE) {
        fputs("location = PAGE_ANYWHERE) -> ", output);
    } else {
        fputs("location = ?) -> ", output);
    }

    if (result == MAP_PAGES_FAILURE) {
        fputs("MAP_PAGES_FAILURE\n", output);
    } else if (result) {
        fprintf(output, "%p\n", result);
    } else {
        fputs("NULL\n", output);
    }
}

void* map_pages(void const* addr, size_t length, enum page_location location) {
    void * result = NULL;

    if (current_map_pages_impl) {
        /* current_map_pages_impl() may or may not call platform_map_pages() itself */
        result = current_map_pages_impl(addr, length, location);
    } else {
        result = platform_map_pages(addr, length, location);
    }

    log_map_pages_call(stderr, addr, length, location, result);
    return result;
}

int unmap_pages(void const* addr, size_t length) {
    return platform_unmap_pages(addr, length);
}

int page_size() {
    return platform_page_size();
}
