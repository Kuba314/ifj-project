/**
 * @file parser.h
 */
#pragma once

#include "ast.h"
#include "parser-generated.h"

/**
 * @brief Consume tokens from lexer and construct an AST
 * @param nterm Starting nterm
 * @param[out] root AST will be constructed there
 */
int parse(nterm_type_t nterm, ast_node_t **root, int depth);

/**
 * @brief Parse expression into AST tree
 * @param[out] root AST will be constructed there
 */
int precedence_parse(ast_node_t **root);

/**
 * @brief Frees AST from memory
 * @param root AST root
 */
void free_ast(ast_node_t *root);

/**
 * @brief Prints AST to stdout
 * @param depth nesting depth
 * @param root AST root
 */
void print_ast(int depth, ast_node_t *root);
