/**
* @file scanner.h
 */

#pragma once

#include "dynstring.h"

/**
 * @enum Enum of reserved keywords.
 */
typedef enum {

} keyword_t;



/**
 * @enum Union of possible token properties.
 */
typedef union {
    string_t *string_value;
    int int_value;
    double double_value;
    keyword_t keyword_value;
} token_prop;

/**
 * @enum Definition of token type
 */
typedef enum {

} token_t;
