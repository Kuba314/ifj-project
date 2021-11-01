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
 * @file deque.h
 *
 * @brief List based deque container
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief deque element
 */
typedef struct deque_element {
    void *data;                 ///< pointer to user data
    struct deque_element *prev; ///< pointer to next
    struct deque_element *next; ///< pointer to next
} deque_element_t;

/**
 * @brief deque containter
 */
typedef struct {
    size_t size;            ///< current size of deque
    deque_element_t *front; ///< pointer to front
    deque_element_t *back;  ///< pointer to back
} deque_t;

/**
 * @brief Creates an empty deque
 *
 * @param deque pointer to deque containter
 * @return E_INT on allocation error, otherwise E_OK
 */
void deque_create(deque_t *deque);

/**
 * @brief Frees deque from memory
 *
 * @param deque pointer to deque containter
 */
void deque_free(deque_t *deque);

/**
 * @brief Pushes data to front of deque
 *
 * @param deque pointer to deque containter
 * @param data pointer to user data
 * @return E_INT on allocation error, otherwise E_OK
 */
int deque_push_front(deque_t *deque, void *data);

/**
 * @brief Removes data from front of deque, frees allocated resources
 *
 * @param deque pointer to deque containter
 * @return NULL if deque is empty, otherwise pointer to user data
 */
void *deque_pop_front(deque_t *deque);

/**
 * @brief Returns pointer to front element
 *
 * @param deque pointer to deque containter
 * @return NULL if deque is empty, otherwise pointer element
 */
deque_element_t *deque_front_element(deque_t *deque);

/**
 * @brief Returns pointer to user data from front
 *
 * @param deque pointer to deque containter
 * @return NULL if deque is empty, otherwise pointer to user data
 */
void *deque_front(deque_t *deque);

/**
 * @brief Returns pointer to back element
 *
 * @param deque pointer to deque containter
 * @return NULL if deque is empty, otherwise pointer element
 */
deque_element_t *deque_back_element(deque_t *deque);

/**
 * @brief Returns pointer to user data from back
 *
 * @param deque pointer to deque containter
 * @return NULL if deque is empty, otherwise pointer to user data
 */
void *deque_back(deque_t *deque);

/**
 * @brief Pushes data to back of deque
 *
 * @param deque pointer to deque containter
 * @param data pointer to user data
 * @return E_INT on allocation error, otherwise E_OK
 */
int deque_push_back(deque_t *deque, void *data);

/**
 * @brief Removes data from back of deque, frees allocated resources
 *
 * @param deque pointer to deque containter
 * @return NULL if deque is empty, otherwise pointer to user data
 */
void *deque_pop_back(deque_t *deque);

/**
 * @brief Inserts data before iterator position
 *
 * @param deque pointer to deque containter
 * @param it pointer to element
 * @param data pointer to user data
 * @return E_INT on allocation error, otherwise E_OK
 */
int deque_insert(deque_t *deque, deque_element_t *it, void *data);

/**
 * @brief Erases at iterator position
 *
 * @param deque pointer to deque containter
 * @param it pointer to element
 * @return E_INT on allocation error, otherwise E_OK
 */
void deque_erase(deque_t *deque, deque_element_t *it);

/**
 * @brief Checks if deque is empty
 *
 * @param deque pointer to deque containter
 * @return true if deque is empty, otherwise false
 */
bool deque_empty(deque_t *deque);
