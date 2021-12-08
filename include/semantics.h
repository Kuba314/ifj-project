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
 * @file semantics.h
 *
 * @brief Semantic analysis
 */
#pragma once

#include "error.h"
#include "ast.h"
#include "parser.h"
#include "parser-generated.h"
#include "symtable.h"

int semantics_init();

void semantics_free();

int sem_get_type(ast_node_t *node, type_t *dest);

int sem_check(ast_node_t *node, int, nut_type_t expected);

bool is_number_or_integer(type_t type);

type_t sem_get_func_call_type(ast_node_t *node);

bool sem_is_builtin_used(char *name);

const char *node_type_to_readable(ast_node_type_t type);
