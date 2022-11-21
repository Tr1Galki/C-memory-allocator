#include "page_routines.h"

void* map_pages(void const* addr, size_t length, enum page_location location) {
    return platform_map_pages(addr, length, location);
}

int unmap_pages(void const* addr, size_t length) {
    return platform_unmap_pages(addr, length);
}

int page_size() {
    return platform_page_size();
}
