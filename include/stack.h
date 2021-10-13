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
 * @file stack.h
 *
 * @brief Simple stack container
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief stack containter
 */
typedef struct {
    size_t size;     ///< current size of stack
    size_t capacity; ///< allocated size of array
    void **array;    ///< array of user data
} adt_stack_t;

/**
 * @brief Creates an empty stack
 *
 * @param stack pointer to stack structure
 * @return E_INT on allocation error, otherwise E_OK
 */
int stack_create(adt_stack_t *stack, size_t size);

/**
 * @brief Frees stack from memory
 *
 * @param stack pointer to stack structure
 */
void stack_free(adt_stack_t *stack);

/**
 * @brief Pushes data to stack
 *
 * @param stack pointer to stack structure
 * @param data pointer to user data
 * @return E_INT on allocation error, otherwise E_OK
 */
int stack_push(adt_stack_t *stack, void *data);

/**
 * @brief Removes data from top of stack
 *
 * @param stack pointer to stack structure
 * @return NULL if stack is empty, otherwise pointer to user data
 */
void *stack_pop(adt_stack_t *stack);

/**
 * @brief Returns pointer to top
 *
 * @param stack pointer to stack structure
 * @return NULL if stack is empty, otherwise pointer to user data
 */
void *stack_top(adt_stack_t *stack);

/**
 * @brief Checks if stack is empty
 *
 * @param stack pointer to stack structure
 * @param index index to array
 * @return true if stack is empty, otherwise false
 */
bool stack_empty(adt_stack_t *stack);
