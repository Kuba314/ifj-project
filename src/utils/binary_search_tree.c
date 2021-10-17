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
 * @file binary_search_tree.c
 *
 * @brief Binary search tree data structure
 */
#include "binary_search_tree.h"

#include <stdlib.h>
#include "error.h"

int bst_insert(bst_node_t **node, const char *key, void *data)
{
    if(*node) {
        int cmp = search_key_comp(&(*node)->key, key);
        if(cmp < 0) {
            bst_insert(&(*node)->left, key, data);
        } else if(cmp > 0) {
            bst_insert(&(*node)->right, key, data);
        } else {
            (*node)->data = data;
        }
    } else {
        bst_node_t *element = malloc(sizeof(bst_node_t));
        if(!element) {
            return E_INT;
        }
        if(search_key_create(&element->key, key) != E_OK) {
            free(node);
            return E_INT;
        }
        element->data = data;
        element->left = NULL;
        element->right = NULL;
        *node = element;
    }
    return E_OK;
}

int bst_find(bst_node_t *node, const char *key, void **data)
{
    if(!node) {
        return E_INT;
    }
    int cmp = search_key_comp(&node->key, key);
    if(cmp < 0) {
        return bst_find(node->left, key, data);
    } else if(cmp > 0) {
        return bst_find(node->right, key, data);
    } else {
        *data = node->data;
        return E_OK;
    }
    return E_INT;
}

static bst_node_t *get_leftmost(bst_node_t *node)
{
    bst_node_t *it = node;
    while(it && it->left) {
        it = it->left;
    }
    return it;
}

int bst_erase(bst_node_t **node, const char *key)
{
    if(!*node) {
        return E_OK;
    }
    int cmp = search_key_comp(&(*node)->key, key);
    if(cmp < 0) {
        bst_erase(&(*node)->left, key);
    } else if(cmp > 0) {
        bst_erase(&(*node)->right, key);
    } else {
        if(!(*node)->left) {
            bst_node_t *replace = (*node)->right;
            search_key_free(&(*node)->key);
            free(*node);
            *node = replace;
        } else if(!(*node)->right) {
            bst_node_t *replace = (*node)->left;
            search_key_free(&(*node)->key);
            free(*node);
            *node = replace;
        } else {
            bst_node_t *replace = get_leftmost((*node)->right);
            search_key_t replace_key;
            if(search_key_create(&replace_key, search_key_value(&replace->key)) != E_OK) {
                return E_INT;
            }
            search_key_free(&(*node)->key);
            (*node)->key = replace_key;
            (*node)->data = replace->data;
            bst_erase(&(*node)->right, search_key_value(&replace->key));
        }
    }
    return E_OK;
}

void bst_free(bst_node_t *node)
{
    if(!node) {
        return;
    }
    if(node->left) {
        bst_free(node->left);
    }
    if(node->right) {
        bst_free(node->right);
    }
    search_key_free(&node->key);
    free(node);
}
