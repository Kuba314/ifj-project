/**
 * IFJ21 Compiler
 *
 *  Copyright 2021 xruzaa00 Adam Ruza
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * @file symtable.h
 *
 * @brief Symbol table implementation (using hashtable)
 */
#pragma once

#include "hashtable_bst.h"
#include "ast.h"

typedef ast_node_t symbol_t;

/**
 * @brief Initializes symtable stack
 *
 * @return E_INT on allocation error, othewise E_OK
 */
int symtable_init();

/**
 * @brief Frees all associated memory
 */
void symtable_free();

/**
 * @brief Creates and pushes scope
 *
 * @return E_INT on allocation error, othewise E_OK
 */
int symtable_push_scope();

/**
 * @brief Pops and releases scope from memory
 *
 * @return E_INT if symtable is empty, othewise E_OK4
 */
int symtable_pop_scope();

/**
 * @brief Inserts symbol into current scope
 *
 * @param identifier string id
 * @param data pointer to symbol data structure
 * @return E_INT on allocation error or empty symtable, othewise E_OK
 */
int symtable_put_symbol(const char *identifier, symbol_t *data);

/**
 * @brief Inserts symbol into global
 *
 * @param identifier string id
 * @param data pointer to symbol data structure
 * @return E_INT on allocation error or empty symtable, othewise E_OK
 */
int symtable_put_in_global(const char *identifier, symbol_t *data);

/**
 * @brief Tries to find symbol in all scopes
 *
 * @param identifier string id
 * @return pointer to symbol data if found, othewise NULL
 */
symbol_t *symtable_find(const char *identifier);

/**
 * @brief Tries to find symbol in global scope
 *
 * @param identifier string id
 * @return pointer to symbol data if found, othewise NULL
 */
symbol_t *symtable_find_in_global(const char *identifier);

/**
 * @brief Tries to find symbol in only current scope
 *
 * @param identifier string id
 * @return pointer to symbol data if found, othewise NULL
 */
symbol_t *symtable_find_in_current(const char *identifier);

/**
 * @brief Appends suffix to create unique identifier
 *
 * @param dest pointer to string to be appended to
 * @return E_INT on allocation error or empty symtable, othewise E_OK
 */
int symtable_create_mangled_id(string_t *dest);
