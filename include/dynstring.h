#pragma once

#include <stddef.h>

#define STRING_ALLOC_LENGTH 16

typedef struct {
    char *ptr;
    size_t alloc_length;
    size_t length;
} string_t;

int str_create_empty(string_t *str);
int str_create(const char *s, string_t *str);
int str_append_char(string_t *str, char ch);
void str_free(string_t *str);
