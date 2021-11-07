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
int parse(nterm_type_t nterm, ast_node_t **root);
