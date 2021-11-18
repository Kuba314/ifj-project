#include "symtable.h"

#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "deque.h"
#include "string.h"
#include "stack.h"

#define DEFAULT_SIZE (101)

typedef struct {
    hashtable_t scope;
    size_t label_counter;
} frame_t;

static deque_t scopes;

static frame_t *global_frame;

static uint64_t hash(const char *key)
{
    uint64_t h = 0;
    for(const char *c = key; *c != '\0'; ++c) {
        h = ((h << 5) ^ (h >> 27)) ^ *c;
    }
    return h;
}

static void free_frame(frame_t *frame)
{
    hashtable_free(&frame->scope);
    free(frame);
}

static frame_t *create_frame()
{
    frame_t *frame = malloc(sizeof(frame_t));
    if(!frame) {
        return NULL;
    }
    if(hashtable_create_bst(&frame->scope, DEFAULT_SIZE, hash) != E_OK) {
        free(frame);
        return NULL;
    }
    frame->label_counter = 0;
    return frame;
}

int symtable_init()
{
    deque_create(&scopes);
    if(symtable_push_scope() != E_OK) {
        return E_INT;
    }
    global_frame = deque_front(&scopes);
    return E_OK;
}

void symtable_free()
{
    deque_element_t *it = deque_front_element(&scopes);
    while(it) {
        free_frame(it->data);
        it = it->next;
    }
    deque_free(&scopes);
}

int symtable_push_scope()
{
    frame_t *frame = create_frame();
    if(!frame) {
        return E_INT;
    }
    return deque_push_front(&scopes, frame);
}

int symtable_pop_scope()
{
    if(deque_empty(&scopes) || deque_front(&scopes) == global_frame) {
        return E_INT;
    }
    frame_t *frame = deque_pop_front(&scopes);
    free_frame(frame);
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
        frame_t *frame = (frame_t *) it->data;
        ast_node_t *symbol = find_in_table(&frame->scope, identifier);
        if(symbol) {
            return symbol;
        }
        it = it->next;
    }
    return NULL;
}

ast_node_t *symtable_find_in_global(const char *identifier)
{
    return find_in_table(&global_frame->scope, identifier);
}

ast_node_t *symtable_find_in_current(const char *identifier)
{
    frame_t *frame = deque_front(&scopes);
    return find_in_table(&frame->scope, identifier);
}

int symtable_create_mangled_id(string_t *dest)
{

    char temp[80];
    sprintf(temp, "%lu", scopes.size - 1);

    for(char *c = temp; *c != '\0'; ++c) {
        if(str_append_char(dest, *c) != 0) {
            return E_INT;
        }
    }

    return E_OK;
}

int symtable_create_mangled_label(string_t *dest)
{
    static const char separator = '&';

    if(symtable_create_mangled_id(dest) != E_OK) {
        return E_INT;
    }
    frame_t *frame = deque_front(&scopes);
    if(!frame) {
        return E_INT;
    }

    char temp[80];
    sprintf(temp, "%c%lu", separator, frame->label_counter);

    for(char *c = temp; *c != '\0'; ++c) {
        if(str_append_char(dest, *c) != 0) {
            return E_INT;
        }
    }
    return E_OK;
}

int symtable_put_symbol(const char *identifier, ast_node_t *data)
{
    frame_t *frame = deque_front(&scopes);
    return hashtable_insert(&frame->scope, identifier, data);
}

int symtable_put_in_global(const char *identifier, ast_node_t *data)
{
    return hashtable_insert(&global_frame->scope, identifier, data);
}

int symtable_scope_level()
{
    return scopes.size - 1;
}

void symtable_increment_label_counter()
{
    frame_t *frame = deque_front(&scopes);
    frame->label_counter++;
}
