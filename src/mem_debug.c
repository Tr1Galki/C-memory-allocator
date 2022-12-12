#include <stdarg.h>
#include <stdio.h>

#include "mem_internals.h"
#include "mem.h"

void debug_struct_info(FILE *f,
                       void const *addr) {

    struct block_header const *header = addr;
    fprintf(f,
            "%10p %10zu %8s   ",
            addr,
            header->capacity.bytes,
            header->is_free ? "free" : "taken"
    );
    for (size_t i = 0; i < DEBUG_FIRST_BYTES && i < header->capacity.bytes; ++i)
        fprintf(f, "%hhX", header->contents[i]);
    fprintf(f, "\n");
}

 block_size size_from_capacity(block_capacity cap) {
    return (block_size) {cap.bytes + offsetof(struct block_header, contents)};
}

 block_capacity capacity_from_size(block_size sz) {
    return (block_capacity) {sz.bytes - offsetof(struct block_header, contents)};
}

 bool region_is_invalid(const struct region *r) { return r->addr == NULL; }



void debug_heap(FILE *f, void const *ptr) {
    fprintf(f, " --- Heap ---\n");
    fprintf(f, "%10s %10s %8s %10s\n", "start", "capacity", "status", "contents");
    for (struct block_header const *header = ptr; header; header = header->next)
        debug_struct_info(f, header);
}

void debug_block(struct block_header *b, const char *fmt, ...) {
#ifdef DEBUG

    va_list args;
    va_start (args, fmt);
    vfprintf(stderr, fmt, args);
    debug_struct_info(stderr, b);
    va_end (args);

#else
    (void) b; (void) fmt;
#endif
}

void debug(const char *fmt, ...) {
#ifdef DEBUG

    va_list args;
    va_start (args, fmt);
    vfprintf(stderr, fmt, args);
    va_end (args);

#else
     (void) fmt;
#endif
}
