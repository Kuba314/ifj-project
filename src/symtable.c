#include "symtable.h"

#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "deque.h"
#include "string.h"
#include "stack.h"

#define DEFAULT_SIZE (101)

static deque_t scopes;

static hashtable_t *global_scope;

static uint64_t hash(const char *key)
{
    uint64_t h = 0;
    for(const char *c = key; *c != '\0'; ++c) {
        h = ((h << 5) ^ (h >> 27)) ^ *c;
    }
    return h;
}

static void free_scope(hashtable_t *scope)
{
    hashtable_free(scope);
    free(scope);
}

int symtable_init()
{
    deque_create(&scopes);
    if(symtable_push_scope() != E_OK) {
        return E_INT;
    }
    global_scope = deque_front(&scopes);
    return E_OK;
}

void symtable_free()
{
    while(!deque_empty(&scopes)) {
        free_scope(deque_pop_front(&scopes));
    }
    deque_free(&scopes);
}

int symtable_push_scope()
{
    hashtable_t *scope = malloc(sizeof(hashtable_t));
    if(hashtable_create_bst(scope, DEFAULT_SIZE, hash) != E_OK) {
        free(scope);
        return E_INT;
    }
    return deque_push_front(&scopes, scope);
}

int symtable_pop_scope()
{
    if(deque_empty(&scopes) || deque_front(&scopes) == global_scope) {
        return E_INT;
    }
    free_scope(deque_pop_front(&scopes));
    return E_OK;
}

static ast_node_t *find_in_table(hashtable_t *scope, const char *identifier)
{
    void *symbol;
    if(hashtable_find(scope, identifier, &symbol) == E_OK) {
        return (ast_node_t *) symbol;
    }
    return NULL;
}

ast_node_t *symtable_find(const char *identifier)
{
    deque_element_t *it = deque_front_element(&scopes);
    while(it) {
        ast_node_t *symbol = find_in_table(it->data, identifier);
        if(symbol) {
            return symbol;
        }
        it = it->next;
    }
    return NULL;
}

ast_node_t *symtable_find_in_global(const char *identifier)
{
    return find_in_table(global_scope, identifier);
}

ast_node_t *symtable_find_in_current(const char *identifier)
{
    hashtable_t *scope = deque_front(&scopes);
    return find_in_table(scope, identifier);
}

int symtable_put_symbol(const char *identifier, ast_node_t *data)
{
    hashtable_t *scope = deque_front(&scopes);
    return hashtable_insert(scope, identifier, data);
}

int symtable_put_in_global(const char *identifier, ast_node_t *data)
{
    return hashtable_insert(global_scope, identifier, data);
}

int symtable_scope_level()
{
    return scopes.size - 1;
}
