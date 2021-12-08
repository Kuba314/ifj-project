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
 * @file optimizations.h
 *
 * @brief Code optimizations
 */
#pragma once

#include "semantics.h"
#include "hashtable_bst.h"

extern bool optimus_prime;

typedef enum
{
    G_GF_OP1,
    G_GF_OP2,
    G_GF_TYPE1,
    G_GF_TYPE2,
    G_GF_STACKRESULT,
    G_GF_STRING0,
    G_GF_STRING1,
    G_GF_LOOP_ITERATOR,
    G_GF_EXPONENT,
    G_GF_BASE,
    G_GF_FOR_ITER,
    G_GF_FOR_CONDITION,
    G_GF_FOR_STEP
} gen_map_t;

int optimize_ast(ast_node_t *node);

bool is_function_used(ast_func_def_t *def);

int gen_usage(ast_node_t *node, hashtable_t *map);

bool gen_is_used(int index);

bool is_not_nil(ast_node_t *node);
