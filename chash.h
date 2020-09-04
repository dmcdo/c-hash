/*
 * Hash table implementation written in C.
 * Will map a key of some type to a value of some type.
 */

#ifndef CHASH
#define CHASH
#define CHASH_TYPE_STRING ((size_t)0)

#include <string.h>

/* Internal structure */
struct __chash_table_entry__
{
    void *key;
    void *val;

    struct __chash_table_entry__ *next;
};

/* Internal structure */
struct __chash_collision_list__
{
    struct __chash_table_entry__ *head;
    size_t depth;
};

struct chash
{
    struct __chash_collision_list__ *table;
    size_t table_size;
    size_t sizeof_val;
    size_t sizeof_key;
};

/* Create an empty hash (you will need to destroy this manually) */
/* Ex: struct chash *hash = chash_new(HASH_TYPE_STRING, sizeof(float)); */
struct chash *chash_create(size_t sizeof_key, size_t sizeof_val);

/* Destroy a hash (Frees all memory allocated and sets pointer to NULL) */
/* Example usage: chash_delete(&hash); */
void chash_destroy(struct chash **p_hash);

/* Insert a key-value pair */
/* Example usage: chash_insert(hashtable, "milk", &price); */
void chash_insert(struct chash *hash, void *key, void *val);

/* Remove a key-value pair */
/* Example usage: chash_remove(hashtable, "milk"); */
void chash_remove(struct chash *hash, void *key);

/* Find a key's corresponding value, returns NULL if the key was not found. */
/* Example usage: float price = *((float *) chash_find(hashtable, "milk")); */
void *chash_find(struct chash *hash, void *key);

/* Function for hashing an object of any type */
long chash_generic_hash(void *obj, size_t sizeof_obj);

#endif /* CHASH */
