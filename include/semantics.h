#pragma once

#include "error.h"
#include "ast.h"
#include "parser.h"
#include "parser-generated.h"
#include "symtable.h"

int sem_init();

void sem_free();

int sem_check(ast_node_t *node, int, nut_type_t expected);

int sem_check_expression(ast_node_t *node);
