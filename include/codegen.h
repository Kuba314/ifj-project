/**
 * IFJ21 Compiler
 *
 *  Copyright 2021 xvanom00 Michal Vano
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * @file codegen.h
 *
 * @brief IFJCode21 generator from an abstract syntax tree
 */

#include "ast.h"

/**
 * @brief Generates code from AST
 *
 * @param ast pointer to the root of the AST
 */
void avengers_assembler(ast_node_t *ast);
