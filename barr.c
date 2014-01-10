/**
 * Copyright (c) 2014, Eddie Ringle <eddie@eringle.net>
 * MIT-licensed. See LICENSE for details.
 */


#include "barr.h"

struct barr_t {
    size_t    size;
    size_t    capacity;
    size_t    chunk_count;
    uint64_t *chunks;
};

#define barr_check(ptr) \
    if ((ptr) == NULL || *(ptr) == NULL || (*(ptr))->chunks == NULL) { \
        return BARR_INVALID_ARGS; \
    };



int barr_init(barr_t *barr)
{
    if (barr == NULL) {
        return BARR_INVALID_ARGS;
    }
    *barr = barr_malloc(sizeof(struct barr_t));
    if (*barr == NULL) {
        return BARR_MEMORY_ERROR;
    }
    (*barr)->chunks = barr_malloc(sizeof(uint64_t));
    if ((*barr)->chunks == NULL) {
        return BARR_MEMORY_ERROR;
    }
    memset((*barr)->chunks, 0, sizeof(uint64_t));
    (*barr)->capacity = BITS_PER_CHUNK;
    (*barr)->chunk_count = 1;
    (*barr)->size = 0;
    return BARR_OK;
}

int barr_destroy(barr_t *barr)
{
    barr_check(barr);
    barr_free((*barr)->chunks);
    barr_free(*barr);
    *barr = NULL;
    return BARR_OK;
}

static int barr_grow(barr_t *barr)
{
    (*barr)->chunks = barr_realloc((*barr)->chunks, sizeof(uint64_t) * (*barr)->chunk_count * 2);
    if ((*barr)->chunks == NULL) {
        return BARR_MEMORY_ERROR;
    }
    memset((*barr)->chunks + (*barr)->chunk_count, 0, sizeof(uint64_t) * (*barr)->chunk_count);
    (*barr)->capacity *= 2;
    (*barr)->chunk_count *= 2;
    return BARR_OK;
}

static int barr_grow_to_index(barr_t *barr, size_t index)
{
    int ret;
    while (index >= (*barr)->capacity) {
        /*
         * TODO:
         * barr_grow memset's the newly allocated portion to zero.
         * We should try to delay the memset until this while loop completes.
         */
        if ((ret = barr_grow(barr)) != BARR_OK) {
            return ret;
        }
    }
    return BARR_OK;
}

int barr_push(barr_t *barr, barr_bit bit)
{
    size_t index = (*barr)->size;
    return (bit) ? barr_set(barr, index) : barr_clear(barr, index);
}

int barr_get(barr_t *barr, size_t index, barr_bit *bit)
{
    barr_check(barr);
    if (bit == NULL) {
        return BARR_INVALID_ARGS;
    }
    if (index >= (*barr)->capacity) {
        *bit = 0;
        return BARR_OK;
    }
    *bit = (unsigned)BIT_CHK((*barr)->chunks[CHUNK_OF_BIT(index)], BIT_OF_CHUNK(index));
    return BARR_OK;
}

int barr_set(barr_t *barr, size_t index)
{
    int ret;
    barr_check(barr);
    if ((ret = barr_grow_to_index(barr, index)) != BARR_OK) {
        return ret;
    }
    BIT_SET((*barr)->chunks[CHUNK_OF_BIT(index)], BIT_OF_CHUNK(index));
    return BARR_OK;
}

int barr_clear(barr_t *barr, size_t index)
{
    barr_check(barr);
    if (index >= (*barr)->capacity) {
        return barr_grow_to_index(barr, index);
    }
    BIT_CLR((*barr)->chunks[CHUNK_OF_BIT(index)], BIT_OF_CHUNK(index));
    return BARR_OK;
}

int barr_toggle(barr_t *barr, size_t index)
{
    int ret;
    barr_check(barr);
    if ((ret = barr_grow_to_index(barr, index)) != BARR_OK) {
        return ret;
    }
    BIT_TGL((*barr)->chunks[CHUNK_OF_BIT(index)], BIT_OF_CHUNK(index));
    return BARR_OK;
}
