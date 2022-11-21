#define _DEFAULT_SOURCE

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem_internals.h"
#include "mem.h"
#include "util.h"

void debug_block(struct block_header* b, const char* fmt, ... );
void debug(const char* fmt, ... );

extern inline block_size size_from_capacity( block_capacity cap );
extern inline block_capacity capacity_from_size( block_size sz );

static bool            block_is_big_enough( size_t query, struct block_header* block ) { return block->capacity.bytes >= query; }
static size_t          pages_count   ( size_t mem )                      { return mem / page_size() + ((mem % page_size()) > 0); }
static size_t          round_pages   ( size_t mem )                      { return page_size() * pages_count( mem ) ; }

void block_init( void* restrict addr, block_size block_sz, void* restrict next ) {
  *((struct block_header*)addr) = (struct block_header) {
    .next = next,
    .capacity = capacity_from_size(block_sz),
    .is_free = true
  };
}

size_t region_actual_size( size_t query ) { return size_max( round_pages( query ), REGION_MIN_SIZE ); }

extern inline bool region_is_invalid( const struct region* r );


/*  аллоцировать регион памяти и инициализировать его блоком */
struct region alloc_region  ( void const * addr, size_t query ) {
    size_t actual_size = region_actual_size(size_from_capacity((block_capacity){query}).bytes);
    void* actual_addr = map_pages(addr, actual_size, PAGE_FIXED);
    if (actual_addr == MAP_PAGES_FAILURE)
        actual_addr = map_pages(addr, actual_size, PAGE_ANYWHERE);
    struct region new_region = {.addr = actual_addr == MAP_PAGES_FAILURE ? NULL : actual_addr,
                                .size = actual_size,
                                .extends = actual_addr == addr};
    if (!region_is_invalid(&new_region))
        block_init(new_region.addr, (block_size){new_region.size}, NULL);
    return new_region;
}

static void* block_after( struct block_header const* block )         ;

void* heap_init( size_t initial ) {
  const struct region region = alloc_region( HEAP_START, initial );
  if ( region_is_invalid(&region) ) return NULL;

  return region.addr;
}

/*  --- Разделение блоков (если найденный свободный блок слишком большой )--- */

static bool block_splittable( struct block_header* restrict block, size_t query) {
  return block-> is_free && query + offsetof( struct block_header, contents ) + BLOCK_MIN_CAPACITY <= block->capacity.bytes;
}

bool split_if_too_big( struct block_header* block, size_t query ) {
    if (!block_splittable(block, query))
        return false;
    void* new_addr = block->contents + query;
    block_init(new_addr, (block_size){block->capacity.bytes - query}, block->next);
    block->capacity.bytes = query;
    block->next = (struct block_header*)(new_addr);
    return true;
}


/*  --- Слияние соседних свободных блоков --- */

static void* block_after( struct block_header const* block )              {
  return  (void*) (block->contents + block->capacity.bytes);
}
static bool blocks_continuous (
                               struct block_header const* fst,
                               struct block_header const* snd ) {
  return (void*)snd == block_after(fst);
}

static bool mergeable(struct block_header const* restrict fst, struct block_header const* restrict snd) {
  return fst->is_free && snd->is_free && blocks_continuous( fst, snd ) ;
}

bool try_merge_with_next( struct block_header* block ) {
    struct block_header* next = block->next;
    if (next == NULL || !mergeable(block, next))
        return false;
    block->capacity.bytes += size_from_capacity(next->capacity).bytes;
    block->next = next->next;
    return true;
}


/*  --- ... ecли размера кучи хватает --- */

struct block_search_result find_good_or_last  ( struct block_header* restrict block, size_t sz )    {
    while (true) {
        if (block->is_free) {
            while (try_merge_with_next(block))
                ;
            if (block_is_big_enough(sz, block))
                return (struct block_search_result){BSR_FOUND_GOOD_BLOCK, block};
        }
        if (block->next == NULL)
            return (struct block_search_result){BSR_REACHED_END_NOT_FOUND, block};
        block = block->next;
    }
}

/*  Попробовать выделить память в куче начиная с блока `block` не пытаясь расширить кучу
 Можно переиспользовать как только кучу расширили. */
struct block_search_result try_memalloc_existing ( size_t query, struct block_header* block ) {
    struct block_search_result search_result = find_good_or_last(block, query);
    if (search_result.type == BSR_FOUND_GOOD_BLOCK) {
        split_if_too_big(search_result.block, query);
        search_result.block->is_free = false;
    }
    return search_result;
}



struct block_header* grow_heap( struct block_header* restrict last, size_t query ) {
    struct region new_region = alloc_region(last->contents + last->capacity.bytes, query);
    if (!region_is_invalid(&new_region) && new_region.extends && last->is_free) {
        last->capacity.bytes += new_region.size;
        return last;
    }
    last->next = new_region.addr;
    return new_region.addr;
}

/*  Реализует основную логику malloc и возвращает заголовок выделенного блока */
struct block_header* memalloc( size_t query, struct block_header* heap_start) {

    query = size_max(query, BLOCK_MIN_CAPACITY);
    struct block_search_result search_result = try_memalloc_existing(query, heap_start);
    if (search_result.type == BSR_REACHED_END_NOT_FOUND) {
        search_result.block = grow_heap(search_result.block, query);
        if (search_result.block)
            search_result = try_memalloc_existing(query, search_result.block);
    }
    return search_result.block;

}

void* _malloc( size_t query ) {
  struct block_header* const addr = memalloc( query, (struct block_header*) HEAP_START );
  if (addr) return addr->contents;
  else return NULL;
}

static struct block_header* block_get_header(void* contents) {
  return (struct block_header*) (((uint8_t*)contents)-offsetof(struct block_header, contents));
}

void _free( void* mem ) {
  if (!mem) return ;
  struct block_header* header = block_get_header( mem );
  header->is_free = true;
    while (try_merge_with_next(header))
        ;
}
