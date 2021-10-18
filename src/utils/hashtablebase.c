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
 * @file hashtablebase.c
 *
 * @brief Hashtable (hashmap) implementation
 */
#include "hashtablebase.h"

#include <stdlib.h>

#include "error.h"

int hashtable_create(hashtable_t *table, size_t size, hash_function_t hash_function,
                     const hash_vtable_t *vtable)
{
    table->array = calloc(sizeof(void *), size);
    if(!table->array) {
        return E_INT;
    }
    table->size = size;
    table->load = 0;
    table->hash_function = hash_function;
    table->vtable = *vtable;
    return E_OK;
}

int hashtable_insert(hashtable_t *table, const char *key, void *data)
{
    size_t index = table->hash_function(key) % table->size;
    if(!table->array[index]) {
        table->load++;
    }
    return table->vtable.node_insert(&table->array[index], key, data);
}

int hashtable_find(hashtable_t *table, const char *key, void **dest)
{
    size_t index = table->hash_function(key) % table->size;
    void *bucket = table->array[index];
    if(!bucket) {
        return E_INT;
    }
    return table->vtable.node_find(bucket, key, dest);
}

void hashtable_free(hashtable_t *table)
{
    if(!table) {
        return;
    }
    for(size_t index = 0; index < table->size; ++index) {
        table->vtable.node_free(table->array[index]);
    }
    free(table->array);
    table->array = NULL;
    table->size = 0;
    table->load = 0;
}

int hashtable_erase(hashtable_t *table, const char *key)
{
    size_t index = table->hash_function(key) % table->size;
    if(table->array[index]) {
        int r = table->vtable.node_erase(&table->array[index], key);
        if(!table->array[index]) {
            table->load--;
        }
        return r;
    }
    return E_OK;
}

float hashtable_load_factor(hashtable_t *table)
{
    return (float) table->load / table->size;
}

bool hashtable_empty(hashtable_t *table)
{
    return table->load == 0;
}
