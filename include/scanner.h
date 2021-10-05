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

/**
 * @struct Definition of token type
 */
typedef struct {
    term_type_t token_type;
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
 * @param FILE Pointer to source file.
 */
void initialise_file_ptr(FILE *source_file);

/**
 * Closes file assigned *fptr.
 *
 * @param FILE Pointer to input stream.
 * @return 0 on success, otherwise return errno.
 */
int close_file(FILE *p);

/**
 * Main function of scanner, which gets the next token from *fptr.
 *
 * @param token_t Pointer to token to be assigned type (and value).
 * @return 0 (E_OK) if tokenization is successful, otherwise 1 (E_LEX) scanner error.
 */
int get_next_token(token_t *t);

/**
 * Saves a token pointer for later use.
 *
 * @param token_t Pointer token to be saved.
 * @return 0 on success, otherwise return 1.
 */
int unget_token(token_t *t);
