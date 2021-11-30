/**
 * IFJ21 Scanner
 *
 *  Copyright 2021 xkrato61 Pavel Kratochvil
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * @file scanner.h
 *
 * @brief Lexical analysis tool
 */
#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "dynstring.h"
#include "parser-generated.h"
#include "error.h"
#include "type.h"

#define TOKEN_BUF_LENGTH 2

/**
 * @struct Definition of token type
 */
typedef struct {
    term_type_t token_type;
    int row, column;
    union {
        string_t string;
        type_t type;
        int64_t integer;
        double number;
        bool boolean;
    };
} token_t;

/**
 * Assigns source file to global variable.
 *
 * @param source_file Pointer to source file to be read from.
 */
void scanner_init(FILE *source_file);

/**
 * Closes file assigned *fptr.
 *
 * @param fp Pointer to input stream.
 * @return E_OK on success, otherwise E_LEX.
 */
int scanner_free(void);

/**
 * Main function of scanner, which gets the next token from *fptr.
 *
 * @param[out] t Pointer to token to be assigned type (and value).
 * @return E_OK if tokenization is successful, otherwise E_LEX or E_INT (malloc err).
 */
int get_next_token(token_t *t);

/**
 * Return up to TOKEN_BUF_LENGTH tokens for later use.
 *
 * @return E_OK on success, otherwise return E_LEX.
 */
int unget_token(void);
