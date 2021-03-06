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
 * @file parser-generated.h
 *
 * @brief terminal and non_terminal definitions, generated by a python script
 */

/*
 * This file was generated by build_grammar.py, DO NOT MODIFY!
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "type.h"
#include "dynstring.h"

// order of these two enums is crucial
typedef enum
{
    NT_FOR_LOOP,
    NT_FUNC_CALL,
    NT_UNOP,
    NT_FUNC_TYPE_LIST,
    NT_IDENTIFIER_LIST_WITH_TYPES,
    NT_GLOBAL_STATEMENT,
    NT_TYPE_LIST2,
    NT_COND_OPT_ELSEIF,
    NT_OPT_RETURN_STATEMENT,
    NT_IDENTIFIER_LIST_WITH_TYPES2,
    NT_STATEMENT,
    NT_FUNC_DECL,
    NT_FUNC_DEF,
    NT_FUN_EXPRESSION_LIST2,
    NT_IDENTIFIER_LIST2,
    NT_TERM,
    NT_OPTIONAL_FOR_STEP,
    NT_PAREN_EXP_LIST_OR_ID_LIST2,
    NT_IDENTIFIER_WITH_TYPE,
    NT_WHILE_LOOP,
    NT_OPTIONAL_FUN_PARENS,
    NT_RETURN_STATEMENT,
    NT_EXPRESSION,
    NT_TYPE_LIST,
    NT_DECL_OPTIONAL_ASSIGNMENT,
    NT_OPTIONAL_FUN_EXPRESSION_LIST,
    NT_RET_EXPRESSION_LIST,
    NT_REPEAT_UNTIL,
    NT_BINOP,
    NT_EXPRESSION_LIST,
    NT_STATEMENT_LIST,
    NT_FUNC_TYPE_LIST2,
    NT_ASSIGNMENT,
    NT_EXPRESSION_LIST2,
    NT_RET_EXPRESSION_LIST2,
    NT_PROGRAM,
    NT_COND_STATEMENT,
    NT_STATEMENT_LIST2,
    NT_OPT_BINOP,
    NT_DECLARATION,
    NT_IDENTIFIER_LIST,
    NT_GLOBAL_STATEMENT_LIST,
} nterm_type_t;

typedef enum
{
    T_FOR,
    T_LTE,
    T_ELSE,
    T_LPAREN,
    T_GTE,
    T_MINUS,
    T_TILDE_EQUALS,
    T_COLON,
    T_STRING,
    T_OR,
    T_DOUBLE_SLASH,
    T_ELSEIF,
    T_DOUBLE_EQUALS,
    T_BREAK,
    T_IDENTIFIER,
    T_RPAREN,
    T_LOCAL,
    T_AND,
    T_UNTIL,
    T_NUMBER,
    T_COMMA,
    T_TYPE,
    T_DO,
    T_SLASH,
    T_PERCENT,
    T_FUNCTION,
    T_WHILE,
    T_REQUIRE,
    T_LT,
    T_GLOBAL,
    T_ASTERISK,
    T_PLUS,
    T_RETURN,
    T_NIL,
    T_BOOL,
    T_GT,
    T_NOT,
    T_THEN,
    T_INTEGER,
    T_REPEAT,
    T_EQUALS,
    T_END,
    T_HASH,
    T_DOUBLE_DOT,
    T_EOF,
    T_CARET,
    T_IF,
} term_type_t;

// nterm U term => nut
typedef struct {
    bool is_nterm;
    union {
        nterm_type_t nterm;
        term_type_t term;
    };
} nut_type_t;

typedef struct {
    size_t size;
    nut_type_t *data;
    bool valid;
} exp_list_t;

// hashmap without collisions
typedef struct {
    size_t bucket_count;
    exp_list_t data[];
} parser_table_t;

size_t parser_get_table_index(nterm_type_t nterm, term_type_t term);
const char *nterm_to_readable(nterm_type_t nterm);
const char *term_to_readable(term_type_t term);
int parser_init();
void parser_free();

extern parser_table_t *table;
