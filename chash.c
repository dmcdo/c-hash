#include "chash.h"
#include <stdlib.h>

#define _CHASH_DEFAULT_SIZE (32)
#define _CHASH_MAX_DEPTH (2)

typedef struct _CHASH_NODE CHASH_NODE;


/*
 * Internal functions
 */
CHASH_KEY_VAL_PAIR *_chash_kvp_static_pointer(CHASH_KEY_VAL_PAIR *p_set_val)
{
    static CHASH_KEY_VAL_PAIR x;

    if (p_set_val != NULL)
        x = *p_set_val;

    return &x;
}

void _chash_set_error(int *p, int error)
{
    if (p != NULL)
        *p = error;
}

void _chash_resize(CHASH *hash, size_t size, int *error)
{
    CHASH_NODE **array = (CHASH_NODE **)calloc(size, sizeof(CHASH_NODE *));

    if (array == NULL)
    {
        _chash_set_error(error, CHASH_MEMORY_ALLOC_ERROR);
        return;
    }

    for (int i = 0; i < hash->_size; i++)
    {
        CHASH_NODE *node;
        CHASH_NODE *next;
        for (node = hash->_array[i]; node != NULL;)
        {
            next = node->_next;

            size_t j = hash->_hash_key(node->_pair.key) % size;
            if (array[j] == NULL)
            {
                array[j] = node;
                array[j]->_next = NULL;
            }
            else
            {
                CHASH_NODE *parent;
                for (parent = array[j]; parent->_next != NULL; parent = parent->_next)
                    ;

                parent->_next = node;
                node->_next = NULL;
            }

            node = next;
        }
    }

    free(hash->_array);
    hash->_array = array;
    hash->_size = size;
}


/*
 * Funtion definitions
 */
CHASH *chash_create(
    size_t sizeof_key,
    size_t sizeof_val,
    size_t (*hash_key_function)(const void *),
    int (*compare_keys_function)(const void *, const void *),
    int *error)
{
    CHASH *hash = (CHASH *)malloc(sizeof(CHASH));
    size_t size = _CHASH_DEFAULT_SIZE;

    if (hash == NULL)
    {
        _chash_set_error(error, CHASH_MEMORY_ALLOC_ERROR);
        return NULL;
    }

    hash->_array = (CHASH_NODE **)calloc(size, sizeof(CHASH_NODE *));

    if (hash->_array == NULL)
    {
        _chash_set_error(error, CHASH_MEMORY_ALLOC_ERROR);
        free(hash);
        return NULL;
    }

    hash->_size = size;
    hash->_sizeof_key = sizeof_key;
    hash->_sizeof_val = sizeof_val;
    hash->_hash_key = hash_key_function;
    hash->_compare_keys = compare_keys_function;

    _chash_set_error(error, CHASH_NO_ERROR);
    return hash;
}

void chash_delete(CHASH *hash, bool free_keys, bool free_vals)
{
    for (int i = 0; i < hash->_size; i++)
    {
        CHASH_NODE *node = hash->_array[i];
        while (node != NULL)
        {
            CHASH_NODE *next = node->_next;

            if (free_keys)
                free(node->_pair.key);
            if (free_vals)
                free(node->_pair.val);

            free(node);
            node = next;
        }
    }

    free(hash->_array);
    free(hash);
}

CHASH_KEY_VAL_PAIR *chash_insert(CHASH *hash, void *key, void *val, int *error)
{
    size_t i = hash->_hash_key(key) % hash->_size;

    /* Check if key already exists in hash. */
    size_t depth = 0;
    CHASH_NODE *parent = NULL;
    CHASH_NODE *node = hash->_array[i];
    while (node != NULL)
    {
        /* If it exists, replace it. */
        if (hash->_compare_keys(key, node->_pair.key) == 0)
        {
            CHASH_KEY_VAL_PAIR *p = _chash_kvp_static_pointer(&node->_pair);

            node->_pair.key = key;
            node->_pair.val = val;
            
            return p;
        }

        parent = node;
        node = node->_next;
        depth++;
    }

    /* Insert node */
    node = (CHASH_NODE *)malloc(sizeof(CHASH_NODE));

    if (node == NULL)
    {
        _chash_set_error(error, CHASH_MEMORY_ALLOC_ERROR);
        return NULL;
    }

    node->_pair.key = key;
    node->_pair.val = val;
    node->_next = NULL;
    if (parent == NULL)
        hash->_array[i] = node;
    else
        parent->_next = node;


    if (depth >= _CHASH_MAX_DEPTH)
    {
        _chash_resize(hash, hash->_size << 1, NULL);
    }

    _chash_set_error(error, CHASH_NO_ERROR);
    return NULL;
}


CHASH_KEY_VAL_PAIR *chash_lookup(CHASH *hash, void *key, int *error)
{
    size_t i = hash->_hash_key(key) % hash->_size;

    for (CHASH_NODE *node = hash->_array[i]; node != NULL; node = node->_next)
    {
        if (hash->_compare_keys(key, node->_pair.key) == 0)
        {
            CHASH_KEY_VAL_PAIR *p = _chash_kvp_static_pointer(&node->_pair);
            return p;
        }
    }

    _chash_set_error(error, CHASH_NO_ERROR);
    return NULL;
}

CHASH_KEY_VAL_PAIR *chash_remove(CHASH *hash, void *key, int *error)
{
    size_t i = hash->_hash_key(key) % hash->_size;

    CHASH_NODE *parent = NULL;
    CHASH_NODE *node = hash->_array[i];
    while (node != NULL)
    {
        /* If it exists, replace it. */
        if (hash->_compare_keys(key, node->_pair.key) == 0)
        {
            if (parent == NULL)
                hash->_array[i] = node->_next;
            else
                parent->_next = node->_next;

            CHASH_KEY_VAL_PAIR *p = _chash_kvp_static_pointer(&node->_pair);
            free(node);
            return p;
        }

        parent = node;
        node = node->_next;
    }

    _chash_set_error(error, CHASH_NO_ERROR);
    return NULL;
}


/* 
 * Iterator Functions
 */

CHASH_ITERATOR chash_iterate_begin(CHASH *hash)
{
    CHASH_ITERATOR iter;
    iter._hash = hash;
    iter.end = false;
    iter.key = NULL;
    iter.val = NULL;
    iter._i = 0;
    iter._p = hash->_array[0];

    chash_iterate_next(&iter);

    return iter;
}

void chash_iterate_next(CHASH_ITERATOR *iter)
{
    if (iter->end)
    {
        return;
    }

    if (iter->_p != NULL)
    {
        iter->key = iter->_p->_pair.key;
        iter->val = iter->_p->_pair.val;
        iter->_p = iter->_p->_next;
        return;
    }

    for (int i = iter->_i + 1; i < iter->_hash->_size; i++)
    {
        if (iter->_hash->_array[i] != NULL)
        {
            iter->key = iter->_hash->_array[i]->_pair.key;
            iter->val = iter->_hash->_array[i]->_pair.val;
            iter->_i = i;
            iter->_p = iter->_hash->_array[i]->_next;
            return;
        }
    }

    iter->end = true;
    iter->key = NULL;
    iter->val = NULL;
    iter->_i = 0;
    iter->_p = NULL;
}


/*
 * Provided comparison and hashing functions.
 */
int chash_strcmp(const void *s1, const void *s2)
{
    return strcmp((const char *)s1, (const char *)s2);
}

int chash_ptrcmp(const void *p1, const void *p2)
{
    return p1 - p2;
}

int chash_intcmp(const void *x, const void *y)
{
    return *(int *)x - *(int *)y;
}

int chash_longcmp(const void *x, const void *y)
{
    return *(long *)x - *(long *)y;
}

size_t chash_strhash(const void *s)
{
    if (s == NULL)
        return 0;

    size_t i = 0;
    size_t x = 1000003;

    for (const char *p = (const char *)s; *p; p++, i++)
    {
        x = (1000003 * x) ^ *p;
    }

    return x ^ i;
}

size_t chash_ptrhash(const void *p)
{
    return (size_t)p;
}

size_t chash_inthash(const void *x)
{
    return *(int *)x;
}

size_t chash_longhash(const void *x)
{
    return *(long *)x;
}
