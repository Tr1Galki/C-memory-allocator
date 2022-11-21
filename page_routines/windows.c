#include "page_routines.h"

#include <assert.h>
#include <windows.h>

void* platform_map_pages(void const* addr, size_t length, enum page_location location) {
    LPVOID base_address = NULL;
    if (location == PAGE_FIXED) {
        base_address = (LPVOID) addr; /* use address hint from argument */
    }

    /* split length (64-bit) into two DWORDs (32-bit) parameters because Win32 */
    DWORD len_low = (DWORD)(length & 0xFFFFFFFFL);
    DWORD len_high = (DWORD)((length >> 32) & 0xFFFFFFFFL);

    HANDLE anon_handle = CreateFileMapping(
        /* Anonymous mapping */ INVALID_HANDLE_VALUE,
        /* Whatever, security stuff*/ NULL,
        PAGE_EXECUTE_READWRITE,
        len_high, len_low, 
        /* Do not give a name to the mapping */ NULL);

    if (anon_handle == NULL) {
        return MAP_PAGES_FAILURE;
    }

    void* map = MAP_PAGES_FAILURE;

    /* if base_address is not NULL, API call will fail if it cannot create a mapping */
    map = MapViewOfFileEx(anon_handle,
        FILE_MAP_READ | FILE_MAP_WRITE,
        /* offset DWORDs */ 0, 0,
        length,
        (LPVOID) addr);

    CloseHandle(anon_handle);

    if (map == NULL) {
        return MAP_PAGES_FAILURE;
    }

    return map;
}

int platform_unmap_pages(void const* addr, size_t length) {
    (void) length;
    if (UnmapViewOfFile(addr)) {
        return 0;
    }
    return -1;
}

int platform_page_size() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (int)si.dwPageSize;
}
