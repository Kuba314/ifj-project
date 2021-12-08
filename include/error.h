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
 * @file error.h
 *
 * @brief Error type definitions
 */
#pragma once

enum error_type
{
    E_OK = 0,  ///< no error
    E_LEX = 1, ///< scanner error
    E_SYN = 2, ///< syntax error

    // sematic errors
    E_UNDEF = 3,     ///< undefined function or variable
    E_REDEF = 3,     ///< redefinition of a function or a variable
    E_ASSIGN = 4,    ///< assignment type incompatibility
    E_TYPE_CALL = 5, ///< invalid number / type of function arguments or return values
    E_TYPE_EXPR = 6, ///< invalid type for operator in expression
    E_SEM = 7,       ///< other semantic errors

    // runtime errors
    E_NIL = 8,     ///< unexpected nil
    E_ZERODIV = 9, ///< division by zero

    E_INT = 99, ///< internal compiler error
};
