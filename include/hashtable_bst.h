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
 * @file hashtable_bst.h
 *
 * @brief Hashtable with BST bucket resolver
 */
#pragma once

#include "binary_search_tree.h"
#include "hashtablebase.h"

static const hash_vtable_t hash_vtable_bst = { (hash_vtable_free) bst_free,
                                               (hash_vtable_insert) bst_insert,
                                               (hash_vtable_find) bst_find,
                                               (hash_vtable_erase) bst_erase };

static inline int hashtable_create_bst(hashtable_t *table, size_t size,
                                       hash_function_t hash_function)
{
    return hashtable_create(table, size, hash_function, &hash_vtable_bst);
}
