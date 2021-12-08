/**
 * IFJ21 Compiler
 *
 *  Copyright 2021 xrozek02 Jakub Rozek
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * @file dynstring.h
 *
 * @brief Resizable string implementation
 */
#pragma once

#include <stddef.h>

/// pre-allocate this many bytes when creating an empty string
#define STRING_ALLOC_LENGTH 16

/**
 * @brief string type definition
 */
typedef struct {
    char *ptr;           ///< pointer to actual string data, null terminated
    size_t alloc_length; ///< size allocated in bytes
    size_t length;       ///< length of the actual string
} string_t;

/**
 * @brief Creates an empty string object
 *
 * @param[out] str pointer to newly created string
 * @return E_INT on allocation error, else E_OK
 */
int str_create_empty(string_t *str);

/**
 * @brief Creates a string object and loads it with a char sequence
 *
 * @param s char sequence to load
 * @param[out] str pointer to newly created string
 * @return E_INT on allocation error, else E_OK
 */
int str_create(const char *s, string_t *str);

/**
 * @brief Appends a char to the string object
 *
 * @param str string object to append the char to
 * @param ch char that gets appended
 * @return E_INT on allocation error, else E_OK
 */
int str_append_char(string_t *str, char ch);

/**
 * @brief Frees memory allocated by a string object
 *
 * @param str string to free
 */
void str_free(string_t *str);
