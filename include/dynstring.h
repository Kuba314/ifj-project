/**
 * @file dynstring.h
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

int str_create_empty(string_t *str);
int str_create(const char *s, string_t *str);
int str_append_char(string_t *str, char ch);
void str_free(string_t *str);
