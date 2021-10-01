/**
* @file scanner.h
 */

#pragma once

#include "dynstring.h"

/**
 * @enum Definition of reserved keywords.
 */
typedef enum {
    // Reserved keywords for program control structure
    KEYWORD_T_IF,
    KEYWORD_T_ELSE,
    KEYWORD_T_DO,
    KEYWORD_T_END,

    KEYWORD_T_FUNCTION,
    KEYWORD_T_GLOBAL,
    KEYWORD_T_LOCAL,
    KEYWORD_T_NIL,

    KEYWORD_T_REQUIRE,
    KEYWORD_T_RETURN,
    KEYWORD_T_WHILE,
    KEYWORD_T_THEN,

    // Reserved keywords for data types
    KEYWORD_T_INTEGER,
    KEYWORD_T_NUMBER,
    KEYWORD_T_STRING
} keyword_t;

/**
 * @enum Definition of token_kind
 */
typedef enum{
    TOKEN_KIND_NONE,           // default kind?
    TOKEN_KIND_KEYWORD,
    TOKEN_KIND_IDENTIFIER,
    TOKEN_KIND_EOL,

    TOKEN_KIND_EOF,
    TOKEN_KIND_BRACKET_LEFT,    // (
    TOKEN_KIND_BRACKET_RIGHT,   // )
    TOKEN_KIND_COMMA,           // ,
    TOKEN_KIND_COLON,           // :

    // Token for various operators
    TOKEN_KIND_PLUS,            // +
    TOKEN_KIND_MINUS,           // -
    TOKEN_KIND_MUL,             // *
    TOKEN_KIND_DIV,             // /
    TOKEN_KIND_INT_DIV,         // //
    TOKEN_KIND_ASSIGN,          // =
    TOKEN_KIND_LESS,            // <
    TOKEN_KIND_LESS_EQ,         // <=
    TOKEN_KIND_GREATER,         // >
    TOKEN_KIND_GREATER_EQ,      // >=
    TOKEN_KIND_EQ,              // ==
    TOKEN_KIND_NOT_EQ,          // ~=
    TOKEN_KIND_HASHTAG,         // #
    TOKEN_KIND_CONCAT           // ..
} token_kind;

/**
 * @enum Definition of token type
 */
typedef enum {
    token_kind kind;            // Kind of token. Keyword, operator, identifier, EOL, EOF...
    union {                     // Stores dynamic string and then specific value after evaluation.
        string_t *string_value;
        keyword_t keyword_value;
        int int_value;
        double double_value;
    };
} token_t;
