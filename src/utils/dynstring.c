/**
 * @file dynstring.c
 */
#include "dynstring.h"

#include <stdlib.h>
#include <string.h>

#include "error.h"

int str_create_empty(string_t *str)
{
    // allocate initial space for string
    str->ptr = malloc(STRING_ALLOC_LENGTH);
    if(str->ptr == NULL) {
        return E_INT;
    }
    str->ptr[0] = '\0';
    str->length = 0;
    str->alloc_length = STRING_ALLOC_LENGTH;
    return E_OK;
}

int str_create(const char *s, string_t *str)
{
    // allocate space for string
    size_t length = strlen(s);
    str->ptr = malloc(length + 1);
    if(str->ptr == NULL) {
        return E_INT;
    }
    str->length = length;
    str->alloc_length = length + 1;
    strcpy(str->ptr, s);
    return E_OK;
}

int str_append_char(string_t *str, char ch)
{
    // new_char + null byte = 2
    if(str->length + 2 > str->alloc_length) {

        /* allocate buffer twice as large
         * note: str->alloc_length can't be equal to 0 unless STRING_ALLOC_LENGTH is set to 0 or an
         * overflow happened
         */
        str->alloc_length *= 2;
        void *tmp = realloc(str->ptr, str->alloc_length);
        if(tmp == NULL) {
            return E_INT;
        }
        str->ptr = tmp;
    }
    str->ptr[str->length++] = ch;
    str->ptr[str->length] = '\0';
    return E_OK;
}

void str_free(string_t *str)
{
    if(str->ptr) {
        free(str->ptr);
    }
}
