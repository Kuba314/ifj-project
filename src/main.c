/*\u9d28\u306e\u7fbd\u8272 \u30b3\u30f3\u30d1\u30a4\u30e9\u30fc\u0020*/
/**
 * IFJ21 Compiler
 *
 *  Copyright 2021
 *
 *  Jakub Rozek (xrozek02)
 *  Adam Ruza (xruzaa00)
 *  Michal Vano (xvanom00)
 *  Pavel Kratochvil (xkrato61)
 *
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * @file main.c
 */

#include <stdio.h>
#include <locale.h>

#include "scanner.h"
#include "parser.h"
#include "semantics.h"
#include "codegen.h"
#include "optimizations.h"

int main()
{
    setlocale(LC_NUMERIC, "C");
    scanner_init(stdin);

    if(semantics_init()) {
        fprintf(stderr, "internal error: couldn't init symtable\n");
        return E_INT;
    }

    if(parser_init()) {
        fprintf(stderr, "internal error: couldn't init parser\n");
        return E_INT;
    }

    ast_node_t *ast = NULL;
    int result = parse(NT_PROGRAM, &ast, 0);
    if(result == E_OK) {
        if(optimus_prime) {
            result = optimize_ast(ast);
        }
    }
    if(result == E_OK) {
        avengers_assembler(ast);
    }

    free_ast(ast);
    parser_free();
    semantics_free();
    scanner_free();

    return result;
}
