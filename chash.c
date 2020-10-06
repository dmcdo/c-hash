#include "chash.h"

#include <stdlib.h>
#include <inttypes.h>

#define DEFAULT_TABLE_SIZE 50

/******************************
* Internal marcos & functions *
******************************/

#define CHASH_KEY_IS_STRING(hash) (hash->sizeof_key == CHASH_TYPE_STRING)
#define CHASH_VAL_IS_STRING(hash) (hash->sizeof_val == CHASH_TYPE_STRING)

void chash_val_modify(struct chash *hash, void *dest, void *src)
{
    if (CHASH_VAL_IS_STRING(hash))
    {
        dest = realloc(dest, (strlen(src) + 1) * sizeof(char));
        strcpy(dest, src);
    }
    else
    {
        memcpy(dest, src, hash->sizeof_val);
    }
}

/*******************************
*                              *
*  Header function definitions *
*                              *
*******************************/

struct chash *chash_create(
    size_t sizeof_key,
    int (*keyhash)(const void *, const size_t),
    int (*keycmp)(const void *, const void *, const size_t),
    size_t sizeof_val)
{
    /* Allocate the memory necessary for an empty hash table */
    struct chash *hash = malloc(sizeof(struct chash));
    hash->table = calloc(DEFAULT_TABLE_SIZE, sizeof(struct __chash_collision_list__));

    /* Initialize the hash */
    hash->table_size = DEFAULT_TABLE_SIZE;
    hash->sizeof_key = sizeof_key;
    hash->keycmp = keycmp;
    hash->keyhash = keyhash;
    hash->sizeof_val = sizeof_val;

    return hash;
}

void chash_destroy(struct chash **p_hash)
{
    struct chash *hash = *p_hash;

    /* Iterate though every list in the list table */
    for (size_t i = 0; i < hash->table_size; i++)
    {
        struct __chash_table_entry__ *current_node = (hash->table[i]).head;
        while (current_node != NULL)
        {
            struct __chash_table_entry__ *next_node = current_node->next;

            free(current_node->key);
            free(current_node->val);
            free(current_node);

            current_node = next_node;
        }
    }

    free(hash->table);
    free(hash);
    *p_hash = NULL;
}

/*
 * Append a new node to the end of the list like so:
 * [???]->[tail] => create [new] => [???]->[tail]->[new]
 * That is, of course, unless a pair with this key already exists,
 * In that case, just set its val to the new val
 */
void chash_insert(struct chash *hash, void *key, void *val)
{
    int i = hash->keyhash(key, hash->sizeof_key) % hash->table_size;
    struct __chash_table_entry__ *current_node = hash->table[i].head;

    if (current_node != NULL)
    {
        while (current_node->next != NULL)
        {
            /* Check if this key already exists in the list */
            if (hash->keycmp(key, current_node->key, hash->sizeof_key) == 0)
            {
                chash_val_modify(hash, current_node->val, val);
                return; /* Nothing left to do */
            }

            current_node = current_node->next;
        }

        /* Check if this key already exists in the list */
        if (hash->keycmp(key, current_node->key, hash->sizeof_key) == 0)
        {
            chash_val_modify(hash, current_node->val, val);
            return; /* Nothing left to do */
        }
    }

    /*
     * Ok, now we're are the end of the list (current_node is the tail)
     * Now we need to create a new node for the new key-value pair
     */
    struct __chash_table_entry__ *new_node = malloc(sizeof(struct __chash_table_entry__));

    if (CHASH_KEY_IS_STRING(hash))
    {
        new_node->key = malloc((strlen(key) + 1) * sizeof(char));
        strcpy(new_node->key, key);
    }
    else
    {
        new_node->key = malloc(hash->sizeof_key);
        memcpy(new_node->key, key, hash->sizeof_key);
    }

    if (CHASH_VAL_IS_STRING(hash))
    {
        new_node->val = malloc((strlen(val) + 1) * sizeof(char));
        strcpy(new_node->val, val);
    }
    else
    {
        new_node->val = malloc(hash->sizeof_val);
        memcpy(new_node->val, val, hash->sizeof_val);
    }

    new_node->next = NULL;

    if (current_node != NULL)
    {
        /* Now append the new node to the end of the list */
        current_node->next = new_node;
    }
    else
    {
        /* Turns out we were at the beginning of the list the whole time! */
        hash->table[i].head = new_node;
    }

    hash->table[i].depth += 1;
}

void chash_remove(struct chash *hash, void *key)
{
    int i = hash->keyhash(key, hash->sizeof_key) % hash->table_size;

    /* The middle node will be removed, and the left connected to the right */
    /* [l]->[m]->[r] => delete [m] => [l]->[r] */
    struct __chash_table_entry__ *left_node = NULL;
    struct __chash_table_entry__ *middle_node = hash->table[i].head;
    struct __chash_table_entry__ *right_node;

    /* Initialize right_node */
    if (middle_node != NULL)
    {
        right_node = middle_node->next;
    }
    else
    {
        right_node = NULL;
    }

    /* Traverse the list looking for our key-value pair */
    while (middle_node != NULL)
    {
        /* Is this the key we're looking for? */
        if (hash->keycmp(key, middle_node->key, hash->sizeof_key) == 0)
        {
            /* free the middle node */
            free(middle_node->key);
            free(middle_node->val);
            free(middle_node);

            if (left_node != NULL)
            {
                left_node->next = right_node;
            }
            else
            {
                hash->table[i].head = right_node;
            }

            hash->table[i].depth -= 1;
            return;
        }

        /* Shift window over one node */
        left_node = middle_node;
        middle_node = right_node;
        if (middle_node != NULL)
        {
            right_node = middle_node->next;
        }
        else
        {
            right_node = NULL;
        }
    }
}

void *chash_find(struct chash *hash, void *key)
{
    int i = hash->keyhash(key, hash->sizeof_key) % hash->table_size;
    struct __chash_table_entry__ *current_node = hash->table[i].head;

    /* Traverse the list looking for value associated with the provided key */
    while (current_node != NULL)
    {
        if (hash->keycmp(key, current_node->key, hash->sizeof_key) == 0)
        {
            /* Found it! */
            return current_node->val;
        }

        current_node = current_node->next;
    }

    /* Didn't find it :( */
    return NULL;
}

/* Based on Python's string hashing algorithm */
int _chash_mem_hash(const void *__obj, size_t __type)
{
    uint8_t *p = (uint8_t *)__obj;
    int x = *p << 7;

    for (size_t i = 1; i < __type; i++)
    {
        x = (1000003 * x) ^ *(p + i);
    }

    return x ^ __type;
}

int _chash_strcmp(const void *__s1, const void *__s2, const size_t __type)
{
    return strcmp((const char *)__s1, (const char *)__s2);
}

int _chash_str_hash(const void *__s, const size_t __size)
{
    return _chash_mem_hash(__s, strlen((const char *)__s));
}