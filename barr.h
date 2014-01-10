/**
 * Copyright (c) 2014, Eddie Ringle <eddie@eringle.net>
 * MIT-licensed. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>

#ifndef barr_malloc
#    define barr_malloc(sz) malloc(sz)
#endif

#ifndef barr_realloc
#    define barr_realloc(ptr, sz) realloc(ptr, sz)
#endif

#ifndef barr_free
#    define barr_free(ptr) free(ptr)
#endif

#define BIT_SET(i, n) (i |= (1 << (n)))
#define BIT_CLR(i, n) (i &= (~(1 << (n))))
#define BIT_TGL(i, n) (i ^= (1 << (n)))

#define BIT_CHK(i, n) (i & (1 << (n)))

#define BITS_PER_CHUNK (sizeof(uint64_t) * 8)

#define CHUNK_OF_BIT(i) (i / BITS_PER_CHUNK)
#define BIT_OF_CHUNK(i) (i % BITS_PER_CHUNK)

typedef int barr_bit;

enum {
    BARR_OK = 0,
    BARR_INVALID_ARGS,
    BARR_MEMORY_ERROR
};

typedef struct barr_t * barr_t;

/**
 * Initialize a new array
 */
int barr_init(barr_t *barr);

/**
 * Free a given array
 */
int barr_destroy(barr_t *barr);

/**
 * Push a new bit onto the end of the array
 */
int barr_push(barr_t *barr, barr_bit bit);

/**
 * Check the value of the bit at the specified index
 */
int barr_get(barr_t *barr, size_t index, barr_bit *bit);

/**
 * Set the bit at the specified index
 */
int barr_set(barr_t *barr, size_t index);

/**
 * Clear the bit at the specified index
 */
int barr_clear(barr_t *barr, size_t index);

/**
 * Toggle the bit at the specified index
 */
int barr_toggle(barr_t *barr, size_t index);
