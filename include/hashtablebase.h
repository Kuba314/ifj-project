/**
 * IFJ21 Compiler
 *
 *  Copyright 2021 xruzaa00 Adam Ruza
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * @file hashtablebase.h
 *
 * @brief Hashtable (hashmap) implementation
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint64_t (*hash_function_t)(const char *key);
typedef void (*hash_vtable_free)(void *node);
typedef int (*hash_vtable_insert)(void **node, const char *key, void *data);
typedef int (*hash_vtable_find)(void *node, const char *key, void **dest);
typedef int (*hash_vtable_erase)(void **node, const char *key);

/**
 * @brief Hashtable Virtual method table
 */
typedef struct {
    hash_vtable_free node_free;     ///< bucket free functionality
    hash_vtable_insert node_insert; ///< bucket insert functionality
    hash_vtable_find node_find;     ///< bucket find functionality
    hash_vtable_erase node_erase;   ///< bucket erase functionality
} hash_vtable_t;

/**
 * @brief Hashtable structure
 */
typedef struct {
    size_t size;                   ///< allocated size
    void **array;                  ///< buckets
    size_t load;                   ///< nubmer of used buckets (0 to size)
    hash_function_t hash_function; ///< hash function dispatch
    hash_vtable_t vtable;          ///< functions dispatch
} hashtable_t;

/**
 * @brief Inserts to hashtable
 *
 * @param table pointer to table
 * @param key key to insert
 * @param user data to insert
 * @return E_INT on allocation error, othewise E_OK
 */
int hashtable_insert(hashtable_t *table, const char *key, void *data);

/**
 * @brief Searches for key in hashtable
 *
 * @param table pointer to table
 * @param key key to find
 * @param data output user data
 * @return E_OK if found, otherwise E_INT
 */
int hashtable_find(hashtable_t *table, const char *key, void **dest);

/**
 * @brief Frees hashtable from memory
 *
 * @param table pointer to table
 */
void hashtable_free(hashtable_t *table);

/**
 * @brief Erases entry from hashtable
 *
 * @param table pointer to table
 * @param key key to erase
 * @return E_INT on allocation error, othewise E_OK
 */
int hashtable_erase(hashtable_t *table, const char *key);

/**
 * @brief Creates hashtable
 *
 * @param table pointer to table
 * @param key key to insert
 * @param size starting size
 * @param hash_function hash function to use
 * @param vtable table for inheritance
 * @return E_INT on allocation error, othewise E_OK
 */
int hashtable_create(hashtable_t *table, size_t size, hash_function_t hash_function,
                     const hash_vtable_t *vtable);

/**
 * @brief Calculates load factor of hashtable
 *
 * @param table pointer to table
 * @return load factor value (0 to 1)
 */
float hashtable_load_factor(hashtable_t *table);

/**
 * @brief Checks if hashtable is empty
 *
 * @param table pointer to table
 * @return true if hashable is empty, otherwise false
 */
bool hashtable_empty(hashtable_t *table);
