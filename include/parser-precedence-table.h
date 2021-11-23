/* THIS FILE IS AUTO GENERATED */
#pragma once
#include "parser-generated.h"
#include <stdint.h>
#include <stdbool.h>
typedef enum
{
    PREC_ZE = 0,
    PREC_LT,
    PREC_GT,
    PREC_EQ,
} precedence_t;
#define TABLE_SIZE (27)
const uint8_t precedence_table[TABLE_SIZE][TABLE_SIZE] = {
    { PREC_LT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT,
      PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT,
      PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT,
      PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_ZE, PREC_ZE, PREC_ZE,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT,
      PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_ZE, PREC_ZE, PREC_ZE,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT,
      PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_ZE, PREC_ZE, PREC_ZE,
      PREC_LT, PREC_GT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT,
      PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT,
      PREC_LT, PREC_EQ, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_ZE },
    { PREC_GT, PREC_LT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_ZE,
      PREC_ZE, PREC_GT, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_GT },
    { PREC_GT, PREC_ZE, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_ZE,
      PREC_ZE, PREC_GT, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_GT },
    { PREC_GT, PREC_ZE, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_ZE,
      PREC_ZE, PREC_GT, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_GT },
    { PREC_GT, PREC_ZE, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_ZE,
      PREC_ZE, PREC_GT, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_GT },
    { PREC_GT, PREC_ZE, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_ZE,
      PREC_ZE, PREC_GT, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_GT },
    { PREC_GT, PREC_ZE, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_ZE,
      PREC_ZE, PREC_GT, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_GT },
    { PREC_GT, PREC_ZE, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT,
      PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_GT, PREC_ZE,
      PREC_ZE, PREC_GT, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_ZE, PREC_GT },
    { PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT,
      PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT,
      PREC_LT, PREC_ZE, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_LT, PREC_ZE }
};
const term_type_t precedence_terminals[] = {
    T_CARET,   T_NOT,          T_ASTERISK, T_SLASH, T_PLUS,          T_MINUS,        T_DOUBLE_DOT,
    T_LT,      T_LTE,          T_GT,       T_GTE,   T_DOUBLE_EQUALS, T_TILDE_EQUALS, T_AND,
    T_OR,      T_DOUBLE_SLASH, T_PERCENT,  T_HASH,  T_LPAREN,        T_RPAREN,       T_IDENTIFIER,
    T_INTEGER, T_NUMBER,       T_STRING,   T_BOOL,  T_NIL,           T_EOF
};
int term_to_index(term_type_t type)
{
    switch(type) {
    case T_CARET:
        return 0;
    case T_NOT:
        return 1;
    case T_ASTERISK:
        return 2;
    case T_SLASH:
        return 3;
    case T_PLUS:
        return 4;
    case T_MINUS:
        return 5;
    case T_DOUBLE_DOT:
        return 6;
    case T_LT:
        return 7;
    case T_LTE:
        return 8;
    case T_GT:
        return 9;
    case T_GTE:
        return 10;
    case T_DOUBLE_EQUALS:
        return 11;
    case T_TILDE_EQUALS:
        return 12;
    case T_AND:
        return 13;
    case T_OR:
        return 14;
    case T_DOUBLE_SLASH:
        return 15;
    case T_PERCENT:
        return 16;
    case T_HASH:
        return 17;
    case T_LPAREN:
        return 18;
    case T_RPAREN:
        return 19;
    case T_IDENTIFIER:
        return 20;
    case T_INTEGER:
        return 21;
    case T_NUMBER:
        return 22;
    case T_STRING:
        return 23;
    case T_BOOL:
        return 24;
    case T_NIL:
        return 25;
    case T_EOF:
        return 26;
    default:
        return -1;
    }
}
bool is_binary_op(term_type_t type)
{
    switch(type) {
    case T_PLUS:
        return true;
    case T_MINUS:
        return true;
    case T_ASTERISK:
        return true;
    case T_SLASH:
        return true;
    case T_DOUBLE_SLASH:
        return true;
    case T_PERCENT:
        return true;
    case T_CARET:
        return true;
    case T_DOUBLE_DOT:
        return true;
    case T_AND:
        return true;
    case T_OR:
        return true;
    case T_LT:
        return true;
    case T_LTE:
        return true;
    case T_GT:
        return true;
    case T_GTE:
        return true;
    case T_DOUBLE_EQUALS:
        return true;
    case T_TILDE_EQUALS:
        return true;
    default:
        return false;
    }
}
bool is_unary_op(term_type_t type)
{
    switch(type) {
    case T_MINUS:
        return true;
    case T_HASH:
        return true;
    case T_NOT:
        return true;
    default:
        return false;
    }
}
const char *term_to_string(term_type_t type)
{
    switch(type) {
    case T_CARET:
        return "T_CARET";
    case T_NOT:
        return "T_NOT";
    case T_ASTERISK:
        return "T_ASTERISK";
    case T_SLASH:
        return "T_SLASH";
    case T_PLUS:
        return "T_PLUS";
    case T_MINUS:
        return "T_MINUS";
    case T_DOUBLE_DOT:
        return "T_DOUBLE_DOT";
    case T_LT:
        return "T_LT";
    case T_LTE:
        return "T_LTE";
    case T_GT:
        return "T_GT";
    case T_GTE:
        return "T_GTE";
    case T_DOUBLE_EQUALS:
        return "T_DOUBLE_EQUALS";
    case T_TILDE_EQUALS:
        return "T_TILDE_EQUALS";
    case T_AND:
        return "T_AND";
    case T_OR:
        return "T_OR";
    case T_DOUBLE_SLASH:
        return "T_DOUBLE_SLASH";
    case T_PERCENT:
        return "T_PERCENT";
    case T_HASH:
        return "T_HASH";
    case T_LPAREN:
        return "T_LPAREN";
    case T_RPAREN:
        return "T_RPAREN";
    case T_IDENTIFIER:
        return "T_IDENTIFIER";
    case T_INTEGER:
        return "T_INTEGER";
    case T_NUMBER:
        return "T_NUMBER";
    case T_STRING:
        return "T_STRING";
    case T_BOOL:
        return "T_BOOL";
    case T_NIL:
        return "T_NIL";
    case T_EOF:
        return "T_EOF";
    default:
        return "N\\A";
    }
}
