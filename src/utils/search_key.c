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
 * @file search_key.c
 *
 * @brief Key wrapper
 */
#include "search_key.h"

#include <string.h>
#include <stdio.h>

#include "error.h"

static int search_key_create_string(search_key_t *key, const char *value)
{
    if(str_create(value, &key->str_key) != 0) {
        return E_INT;
    }
    return E_OK;
}

int search_key_create(search_key_t *key, const char *value)
{
    return search_key_create_string(key, value);
}

void search_key_free(search_key_t *key)
{
    if(key) {
        str_free(&key->str_key);
    }
}

int search_key_comp(search_key_t *key, const char *value)
{
    return strcmp(value, key->str_key.ptr);
}

void *search_key_value(search_key_t *key)
{
    return key->str_key.ptr;
}
