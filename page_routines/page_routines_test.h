#ifndef _PAGE_ROUTINES_TEST_H
#define _PAGE_ROUTINES_TEST_H

#include "page_routines.h"

#include <stddef.h>

typedef void * (*map_pages_impl)(void const*, size_t, enum page_location);

static map_pages_impl current_map_pages_impl = NULL;

#define DEFINE_MAP_PAGES_IMPL(_name) \
    static void * map_pages_impl_##_name(void const* addr, size_t length, enum page_location location)

#define USE_MAP_PAGES_IMPL(_name) current_map_pages_impl = map_pages_impl_##_name

#endif
