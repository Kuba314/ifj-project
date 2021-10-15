#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "dynstring.h"
#include "type.h"

typedef enum
{
    AST_NODE_INVALID,

    AST_NODE_FUNC_DECL,
    AST_NODE_FUNC_DEF,
    AST_NODE_FUNC_CALL,
    AST_NODE_DECLARATION,
    AST_NODE_ASSIGNMENT,

    AST_NODE_PROGRAM,

    AST_NODE_BODY,
    AST_NODE_ID_TYPE_PAIR,

    AST_NODE_IF,
    AST_NODE_WHILE,
    AST_NODE_FOR,
    AST_NODE_REPEAT,

    AST_NODE_BREAK,
    AST_NODE_RETURN,

    AST_NODE_BINOP,
    AST_NODE_UNOP,

    AST_NODE_TYPE,

    AST_NODE_INTEGER,
    AST_NODE_NUMBER,
    AST_NODE_BOOLEAN,
    AST_NODE_IDENTIFIER,
    AST_NODE_STRING,
    AST_NODE_NIL,
} ast_node_type_t;

typedef enum
{
    AST_NODE_BINOP_ADD,
    AST_NODE_BINOP_SUB,
    AST_NODE_BINOP_MUL,
    AST_NODE_BINOP_DIV,
    AST_NODE_BINOP_INTDIV,
    AST_NODE_BINOP_MOD,
    AST_NODE_BINOP_POWER,

    AST_NODE_BINOP_LT,
    AST_NODE_BINOP_GT,
    AST_NODE_BINOP_LTE,
    AST_NODE_BINOP_GTE,
    AST_NODE_BINOP_EQ,
    AST_NODE_BINOP_NE,

    AST_NODE_BINOP_AND,
    AST_NODE_BINOP_OR,
    AST_NODE_BINOP_CONCAT,
} ast_node_binop_type_t;

typedef enum
{
    AST_NODE_UNOP_NEG,
    AST_NODE_UNOP_LEN,
    AST_NODE_UNOP_NOT,
} ast_node_unop_type_t;

typedef struct ast_node ast_node_t;
typedef ast_node_t *ast_node_list_t;

// declarations of different ast_node types
typedef struct {
    string_t require;
    ast_node_list_t global_statement_list;
} ast_program_t;

typedef struct {
    ast_node_list_t statements;
} ast_body_t;

typedef struct {
    ast_node_t *name;
    ast_node_list_t argument_types;
    ast_node_list_t return_types;
} ast_func_decl_t;

typedef struct {
    ast_node_t *name;
    ast_node_list_t arguments;
    ast_node_list_t return_types;
    ast_node_t *body;
} ast_func_def_t;

typedef struct {
    ast_node_list_t conditions; // if and elseif conditions
    ast_node_list_t bodies;     // if, elseif and else bodies (one node longer than conditions)
} ast_if_t;

typedef struct {
    ast_node_t *condition;
    ast_node_t *body;
} ast_while_t;

typedef struct {
    ast_node_t *body;
    ast_node_t *condition;
} ast_repeat_t;

typedef struct {
    ast_node_t *iterator;
    ast_node_t *setup;
    ast_node_t *condition;
    ast_node_t *step;
    ast_node_t *body;
} ast_for_t;

typedef struct {
    ast_node_t *id_type_pair;
    ast_node_t *assignment;
} ast_declaration_t;

typedef struct {
    ast_node_list_t identifiers;
    ast_node_list_t expressions;
} ast_assignment_t;

typedef struct {
    ast_node_t *name;
    ast_node_list_t arguments;
} ast_func_call_t;

typedef struct {
    ast_node_binop_type_t type;
    ast_node_t *left;
    ast_node_t *right;
} ast_binop_t;

typedef struct {
    ast_node_unop_type_t type;
    ast_node_t *operand;
} ast_unop_t;

typedef struct {
    ast_node_list_t values;
} ast_return_t;

struct ast_node {
    ast_node_type_t node_type;
    union {
        ast_program_t program;

        ast_body_t body;

        ast_func_decl_t func_decl;
        ast_func_def_t func_def;

        ast_if_t if_condition;
        ast_while_t while_loop;
        ast_for_t for_loop;
        ast_repeat_t repeat_loop;

        ast_declaration_t declaration;
        ast_assignment_t assignment;
        ast_func_call_t func_call;
        ast_binop_t binop;
        ast_unop_t unop;
        ast_return_t return_values;
        struct {
            string_t id;
            type_t type;
        } id_type_pair;

        uint64_t integer;
        double number;
        bool boolean;
        string_t identifier;
        string_t string;
        type_t type;
    };
    ast_node_t *next;
    int visited_children;
};
