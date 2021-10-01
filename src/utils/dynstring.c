/**
 * @file dynstring.c
 */
#include "dynstring.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Creates an empty string object
 *
 * @param[out] str pointer to newly created string
 * @return 1 on allocation error, else 0
 */
int str_create_empty(string_t *str)
{
    // allocate initial space for string
    str->ptr = malloc(STRING_ALLOC_LENGTH);
    if(str->ptr == NULL) {
        return 1;
    }
    str->ptr[0] = '\0';
    str->length = 0;
    str->alloc_length = STRING_ALLOC_LENGTH;
    return 0;
}

/**
 * @brief Creates a string object and loads it with a char sequence
 *
 * @param s char sequence to load
 * @param[out] str pointer to newly created string
 * @return 1 on allocation error, else 0
 */
int str_create(const char *s, string_t *str)
{
    // allocate space for string
    size_t length = strlen(s);
    str->ptr = malloc(length + 1);
    if(str->ptr == NULL) {
        return 1;
    }
    str->length = length;
    str->alloc_length = length + 1;
    strcpy(str->ptr, s);
    return 0;
}

/**
 * @brief Appends a char to the string object
 *
 * @param str string object to append the char to
 * @param ch char that gets appended
 * @return 1 on allocation error, else 0
 */
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
            return 1;
        }
        str->ptr = tmp;
    }
    str->ptr[str->length++] = ch;
    str->ptr[str->length] = '\0';
    return 0;
}

/**
 * @brief Frees memory allocated by a string object
 *
 * @param str string to free
 */
void str_free(string_t *str)
{
    if(str->ptr) {
        free(str->ptr);
    }
}