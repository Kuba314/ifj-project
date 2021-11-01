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
 * @file deque.c
 *
 * @brief List based deque container
 */
#include "deque.h"
#include "error.h"

#include <stdlib.h>

void deque_create(deque_t *deque)
{
    deque->size = 0;
    deque->front = NULL;
    deque->back = NULL;
}

void deque_free(deque_t *deque)
{
    deque_element_t *it = deque->front;
    while(it != NULL) {
        deque_element_t *temp = it;
        it = it->next;
        free(temp);
    }
    deque->size = 0;
    deque->front = NULL;
    deque->back = NULL;
}

int deque_push_front(deque_t *deque, void *data)
{
    deque_element_t *element = malloc(sizeof(deque_element_t));
    if(!element) {
        return E_INT;
    }
    element->data = data;
    element->next = deque->front;
    element->prev = NULL;
    if(deque->front) {
        deque->front->prev = element;
    } else {
        deque->back = element;
    }
    deque->front = element;
    deque->size++;
    return E_OK;
}

void *deque_pop_front(deque_t *deque)
{
    if(!deque->front) {
        return NULL;
    }
    deque_element_t *element = deque->front;
    void *data = element->data;
    deque->front = element->next;
    // free resources
    free(element);
    deque->size--;
    if(deque->size == 0) {
        deque->back = NULL;
    }
    return data;
}

int deque_push_back(deque_t *deque, void *data)
{
    deque_element_t *element = malloc(sizeof(deque_element_t));
    if(!element) {
        return E_INT;
    }
    element->data = data;
    element->prev = deque->back;
    element->next = NULL;
    if(deque->back) {
        deque->back->next = element;
    } else {
        deque->front = element;
    }
    deque->back = element;
    deque->size++;
    return E_OK;
}

void *deque_pop_back(deque_t *deque)
{
    if(!deque->back) {
        return NULL;
    }
    deque_element_t *element = deque->back;
    void *data = element->data;
    deque->back = element->prev;
    // free resources
    free(element);
    deque->size--;
    if(deque->size == 0) {
        deque->front = NULL;
    }
    return data;
}

int deque_insert(deque_t *deque, deque_element_t *it, void *data)
{
    if(!it) {
        return deque_push_back(deque, data);
    }
    deque_element_t *element = malloc(sizeof(deque_element_t));
    if(!element) {
        return E_INT;
    }

    deque_element_t *prev = it->prev;
    if(prev) {
        prev->next = element;
    } else {
        deque->front = element;
    }

    it->prev = element;
    element->data = data;
    element->prev = prev;
    element->next = it;
    deque->size++;
    return E_OK;
}

void deque_erase(deque_t *deque, deque_element_t *it)
{
    if(!it) {
        return;
    }

    deque_element_t *prev = it->prev;
    deque_element_t *next = it->next;
    if(prev) {
        prev->next = next;
    } else {
        deque->front = next;
    }
    if(next) {
        next->prev = prev;
    } else {
        deque->back = prev;
    }
    free(it);
    deque->size--;
}

deque_element_t *deque_front_element(deque_t *deque)
{
    return deque->front;
}

void *deque_front(deque_t *deque)
{
    return deque->front ? deque->front->data : NULL;
}

deque_element_t *deque_back_element(deque_t *deque)
{
    return deque->back;
}

void *deque_back(deque_t *deque)
{
    return deque->back ? deque->back->data : NULL;
}

bool deque_empty(deque_t *deque)
{
    return deque->size == 0;
}
