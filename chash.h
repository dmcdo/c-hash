#ifndef CHASH_H
#define CHASH_H

#define CHASH_NO_ERROR (0)
#define CHASH_MEMORY_ALLOC_ERROR (1)

#include <string.h>
#include <stdbool.h>

int chash_strcmp(const void *, const void *);
size_t chash_strhash(const void *);

struct _CHASH_KEY_VAL_PAIR
{
    void *key;
    void *val;
};

struct _CHASH_NODE
{
    struct _CHASH_KEY_VAL_PAIR _pair;
    struct _CHASH_NODE *_next;
};

struct _CHASH
{
    struct _CHASH_NODE **_array;
    size_t _size;
    size_t _sizeof_key;
    size_t _sizeof_val;
    size_t (*_hash_key)(const void *);
    int (*_compare_keys)(const void *, const void *);
};

struct _CHASH_ITERATOR
{
    bool end;
    void *key;
    void *val;
    struct _CHASH *_hash;
    size_t _i;
    struct _CHASH_NODE *_p;
};

typedef struct _CHASH CHASH;
typedef struct _CHASH_ITERATOR CHASH_ITERATOR;
typedef struct _CHASH_KEY_VAL_PAIR CHASH_KEY_VAL_PAIR;

CHASH *chash_create(
    size_t sizeof_key,
    size_t sizeof_val,
    size_t (*hash_key_function)(const void *),
    int (*compare_keys_function)(const void *, const void *),
    int *error
);

void chash_delete(CHASH *hash, bool free_keys, bool free_vals);

CHASH_KEY_VAL_PAIR *chash_insert(CHASH *hash, void *key, void *val, int *error);

CHASH_KEY_VAL_PAIR *chash_lookup(CHASH *hash, void *key, int *error);

CHASH_KEY_VAL_PAIR *chash_remove(CHASH *hash, void *key, int *error);


CHASH_ITERATOR chash_iterate_begin(CHASH *hash);
void chash_iterate_next(CHASH_ITERATOR *iter);

#endif /* CHASH_H */
