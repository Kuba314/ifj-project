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
 * @file binary_search_tree.h
 *
 * @brief Binary search tree data structure
 */
#pragma once

#include <stdbool.h>

#include "search_key.h"

/**
 * @brief BST Tree node
 */
typedef struct bst_node {
    void *data;             ///< user data
    search_key_t key;       ///< key
    struct bst_node *left;  ///< pointer to left node
    struct bst_node *right; ///< pointer to right node
} bst_node_t;

/**
 * @brief Inserts to BST Tree
 *
 * @param tree pointer to tree
 * @param key key to insert
 * @param user data to insert
 * @return E_INT on allocation error, othewise E_OK
 */
int bst_insert(bst_node_t **node, const char *key, void *data);

/**
 * @brief Erases from BST Tree
 *
 * @param tree pointer to tree
 * @param key key to erase
 * @return E_INT on allocation error, othewise E_OK
 */
int bst_erase(bst_node_t **node, const char *key);

/**
 * @brief Frees BST from memory
 *
 * @param node pointer to root node
 */
void bst_free(bst_node_t *node);

/**
 * @brief Searches for key in tree
 *
 * @param root pointer to root node
 * @param key key to find
 * @param data output user data
 * @return E_OK if found, otherwise E_INT
 */
int bst_find(bst_node_t *node, const char *key, void **data);
