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
 * @file type.h
 *
 * @brief variable type definition
 */
#pragma once

typedef enum
{
    TYPE_INTEGER,
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_NIL
} type_t;

const char *type_to_readable(type_t type);
