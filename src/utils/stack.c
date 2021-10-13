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
 * @file stack.c
 *
 * @brief Simple stack container
 */
#include "stack.h"
#include "error.h"

#include <stdlib.h>

int stack_create(adt_stack_t *stack, size_t size)
{
    stack->array = malloc(size * sizeof(void *));
    if(!stack->array) {
        return E_INT;
    }
    stack->size = 0;
    stack->capacity = size;

    return E_OK;
}

void stack_free(adt_stack_t *stack)
{
    free(stack->array);
    stack->size = 0;
    stack->capacity = 0;
}

int stack_push(adt_stack_t *stack, void *data)
{
    if(stack->size == stack->capacity) { // stack is full, extend array
        stack->capacity *= 2;
        void *temp = realloc(stack->array, stack->capacity * sizeof(void *));
        if(!temp) {
            return E_INT;
        }
        stack->array = temp;
    }

    stack->array[stack->size++] = data;
    return E_OK;
}

void *stack_pop(adt_stack_t *stack)
{
    return stack->size == 0 ? NULL : stack->array[--stack->size];
}

void *stack_top(adt_stack_t *stack)
{
    return stack_empty(stack) ? NULL : stack->array[stack->size - 1];
}

bool stack_empty(adt_stack_t *stack)
{
    return stack->size == 0;
}
