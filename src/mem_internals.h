#ifndef _MEM_INTERNALS_
#define _MEM_INTERNALS_

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#define BLOCK_MIN_CAPACITY 24

#define REGION_MIN_SIZE (2 * 4096)

struct region { void* addr; size_t size; bool extends; };
static const struct region REGION_INVALID = {0};

inline bool region_is_invalid( const struct region* r ) { return r->addr == NULL; }

typedef struct { size_t bytes; } block_capacity;
typedef struct { size_t bytes; } block_size;

struct block_header {
  struct block_header*    next;
  block_capacity capacity;
  bool           is_free;
  uint8_t        contents[];
};

struct block_search_result {
  enum {BSR_FOUND_GOOD_BLOCK, BSR_REACHED_END_NOT_FOUND, BSR_CORRUPTED} type;
  struct block_header* block;
};

inline block_size size_from_capacity( block_capacity cap ) { return (block_size) {cap.bytes + offsetof( struct block_header, contents ) }; }
inline block_capacity capacity_from_size( block_size sz ) { return (block_capacity) {sz.bytes - offsetof( struct block_header, contents ) }; }
size_t region_actual_size( size_t query );

void block_init( void* addr, block_size block_sz, void* next );
struct region alloc_region  ( void const * addr, size_t query );
struct block_search_result find_good_or_last  ( struct block_header* block, size_t sz );
bool split_if_too_big( struct block_header* block, size_t query );
struct block_header* grow_heap( struct block_header* last, size_t query );
bool try_merge_with_next( struct block_header* block );
struct block_search_result try_memalloc_existing ( size_t query, struct block_header* block );
struct block_header* memalloc( size_t query, struct block_header* heap_start);

#endif
