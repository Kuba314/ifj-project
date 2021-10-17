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
 * @file search_key.h
 *
 * @brief Key wrapper
 */
#pragma once

#include "dynstring.h"

/**
 * @brief Search key structure
 */
typedef struct {
    string_t str_key; ///< dynamic string representing key
} search_key_t;

/**
 * @brief Creates key structure
 *
 * @param key pointer to struct
 * @param value string data
 * @return E_INT on allocation error or empty symtable, othewise E_OK
 */
int search_key_create(search_key_t *key, const char *value);

/**
 * @brief Frees key data from memory
 *
 * @param key pointer to struct
 */
void search_key_free(search_key_t *key);

/**
 * @brief Compares key with string
 *
 * @param key pointer to struct
 * @param value string data
 * @return 0 if equals, -1 if value is less than key, 1 if value is greater than key
 */
int search_key_comp(search_key_t *key, const char *value);

/**
 * @brief Return pointer to key value
 *
 * @param key pointer to struct
 * @return pointer to key value
 */
void *search_key_value(search_key_t *key);
