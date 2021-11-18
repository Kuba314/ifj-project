#include "symtable.h"

#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "deque.h"
#include "string.h"

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
    deque_element_t *it = deque_front_element(&scopes);
    while(it) {
        hashtable_free(it->data);
        free(it->data);
        it = it->next;
    }
    deque_free(&scopes);
}

int symtable_push_scope()
{
    hashtable_t *scope = malloc(sizeof(hashtable_t));
    if(!scope) {
        return E_INT;
    }
    if(hashtable_create_bst(scope, DEFAULT_SIZE, hash) != E_OK) {
        free(scope);
        return E_INT;
    }
    if(deque_push_front(&scopes, scope) != E_OK) {
        hashtable_free(scope);
        free(scope);
        return E_INT;
    }
    return E_OK;
}

int symtable_pop_scope()
{
    if(deque_empty(&scopes)) {
        return E_INT;
    }
    hashtable_t *scope = deque_pop_front(&scopes);
    hashtable_free(scope);
    free(scope);
    return E_OK;
}

static symbol_t *find_in_table(hashtable_t *scope, const char *identifier)
{
    void *symbol;
    if(hashtable_find(scope, identifier, &symbol) == E_OK) {
        return (symbol_t *) symbol;
    }
    return NULL;
}

symbol_t *symtable_find(const char *identifier)
{
    deque_element_t *it = deque_front_element(&scopes);
    while(it) {
        symbol_t *symbol = find_in_table(it->data, identifier);
        if(symbol) {
            return symbol;
        }
        it = it->next;
    }
    return NULL;
}

symbol_t *symtable_find_in_global(const char *identifier)
{
    return find_in_table(global_scope, identifier);
}

symbol_t *symtable_find_in_current(const char *identifier)
{
    return find_in_table(deque_front(&scopes), identifier);
}

int symtable_create_mangled_id(string_t *dest)
{

    char temp[80];
    sprintf(temp, "#%lu", scopes.size - 1);

    for(char *c = temp; *c != '\0'; ++c) {
        if(str_append_char(dest, *c) != 0) {
            return E_INT;
        }
    }

    return E_OK;
}

int symtable_put_symbol(const char *identifier, symbol_t *data)
{
    hashtable_t *scope = deque_front(&scopes);
    return hashtable_insert(scope, identifier, data);
}

int symtable_put_in_global(const char *identifier, symbol_t *data)
{
    return hashtable_insert(global_scope, identifier, data);
}
