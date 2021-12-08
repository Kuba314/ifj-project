/**
 * IFJ21 Compiler
 *
 *  Copyright 2021 xrozek02 Jakub Rozek
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * @file type.c
 *
 * @brief defines a function to convert variable type to string
 */
#include "type.h"

const char *type_to_readable(type_t type)
{
    switch(type) {
    case TYPE_INTEGER:
        return "integer";
    case TYPE_NUMBER:
        return "number";
    case TYPE_STRING:
        return "string";
    case TYPE_BOOL:
        return "boolean";
    case TYPE_NIL:
        return "nil";
    }
    return "unknown-type";
}
