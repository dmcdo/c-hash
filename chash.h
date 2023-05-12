#ifndef CHASH_H
#define CHASH_H

#define CHASH_NO_ERROR (0)
#define CHASH_MEMORY_ALLOC_ERROR (1)

#include <string.h>
#include <stdbool.h>

/* Provided comparison functions */
int chash_strcmp(const void *, const void *);
int chash_ptrcmp(const void *, const void *);
int chash_intcmp(const void *, const void *);
int chash_longcmp(const void *, const void *);

/* Provided hash functions */
size_t chash_strhash(const void *);
size_t chash_ptrhash(const void *);
size_t chash_inthash(const void *);
size_t chash_longhash(const void *);


/* Strucutres using internally */
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


/*
 * Functions
 */

/* Create an empty CHASH object */
CHASH *chash_create(
    size_t sizeof_key,
    size_t sizeof_val,
    size_t (*hash_key_function)(const void *),
    int (*compare_keys_function)(const void *, const void *),
    int *error
);

/* 
 * CHASH objects are stored on the heap. Use this function to free any
 * alloc'd memory. Set free_keys to true to also call free() on the pointers
 * to key values. Set free_vals to do the same to value pointers.
 */
void chash_delete(CHASH *hash, bool free_keys, bool free_vals);

/* 
 * Insert a Key-Value pair into a CHASH.
 * If a value with that key already exists in the hash, it will be returned,
 * otherwise, NULL will be returned.
 */
CHASH_KEY_VAL_PAIR *chash_insert(CHASH *hash, void *key, void *val, int *error);

/* Look up a key, return the pair stored. */
CHASH_KEY_VAL_PAIR *chash_lookup(CHASH *hash, void *key, int *error);

/* Remove a key, return the pair stored. */
CHASH_KEY_VAL_PAIR *chash_remove(CHASH *hash, void *key, int *error);

CHASH_ITERATOR chash_iterate_begin(CHASH *hash); // Create a new iterator
void chash_iterate_next(CHASH_ITERATOR *iter);   // Increment the iterator

#endif /* CHASH_H */
