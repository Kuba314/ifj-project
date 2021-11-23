#pragma once

#include "scanner.h"
#include "parser-generated.h"

// mock function pair for testing

int token_gen(token_t *token);
void token_unget(token_t *token);
