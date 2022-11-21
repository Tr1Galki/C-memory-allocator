#ifndef _PAGE_ROUTINES_H
#define _PAGE_ROUTINES_H

#include <stddef.h>

#define MAP_PAGES_FAILURE ((void*)-1)

enum page_location {
    PAGE_FIXED = 0,
    PAGE_ANYWHERE
};

/* platform-specific routines for page access */
void* platform_map_pages(void const* addr, size_t length, enum page_location location);
int platform_unmap_pages(void const* addr, size_t length);
int platform_page_size();

/* wrapped calls: for tests, those are used to reimplement or intercept actual API calls */
void* map_pages(void const* addr, size_t length, enum page_location location);
int unmap_pages(void const* addr, size_t length);
int page_size();

#endif
