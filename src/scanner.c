/**
 * IFJ21 Scanner
 *
 *  Copyright 2021 xkrato61 Pavel Kratochvil
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * @file scanner.c
 *
 * @brief Lexical analysis tool
 */

#include "scanner.h"
#include "parser-generated.h"
#include "type.h"
#include "string.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum
{
    SCANNER_STATE_START,              ///> Starting state, where scanner returns after every token
    SCANNER_STATE_KEYWORD_IDENTIFIER, ///> State for a possible identifier or reserved keyword
    SCANNER_STATE_NUMBER,             ///> State for reading integer digits
    SCANNER_STATE_DECIMAL,            ///> State for decimal digits
    SCANNER_STATE_EXPONENT,           ///> State for "e" or "E"
    SCANNER_STATE_EXPONENT_VALUE,     ///> State for exponent value, ONLY integer exponent value is
                                      /// accepted

    SCANNER_STATE_COMMENT_DASH_1, ///> State for the first dash at the beginning of commentary
    SCANNER_STATE_COMMENT_DASH_2, ///> State for the second dash at the beginning of commentary
    SCANNER_STATE_INLINE_COMMENT,
    SCANNER_STATE_BLOCK_COMMENT_BRACKET, ///> State for the first square opening bracket
    SCANNER_STATE_BLOCK_COMMENT,         ///> State for insides of block comment to ignore
    SCANNER_STATE_CLOSING_BRACKET,       ///> State for the first closing bracket of block comment

    SCANNER_STATE_STRING,          ///> State for reading string
    SCANNER_STATE_ESCAPE_CHAR_SEQ, ///> State for reading escape character or first digit from
                                   /// escape decimal sequence
    SCANNER_STATE_ESCAPE_SEQ_1,    ///> State for reading second degit from escape decimal sequence
    SCANNER_STATE_ESCAPE_SEQ_2,    ///> State for reading third degit from escape decimal sequence

    SCANNER_STATE_DOT,          ///> State for the first '.' at the beginning of '..' operator
    SCANNER_STATE_LESS_THAN,    ///> State for '<' at the beginning of '<=' operator
    SCANNER_STATE_GREATER_THAN, ///> State for the first '=' at the beginning of '==' operator
    SCANNER_STATE_EQUALS,       ///> State for '>' at the beginning of '>=' operator
    SCANNER_STATE_TILDE,        ///> State for '~' at the beginning of '~=' operator
    SCANNER_STATE_SLASH         ///> State for '/' at the beginning of '//' operator

} scanner_state;

static FILE *fptr;
static token_t last_tokens[TOKEN_BUF_LENGTH];
static short unsigned last_token_ix = 0;
static int row, column;

/**
 * Identifies keyword.
 *
 *
 * @param[out] t Pointer token, to which attributes are assigned
 * @param str Pointer to string containing identifier name or reserved keyword.
 */
static void identify_keyword(string_t *str, token_t *t)
{
    if(!strcmp(str->ptr, "if")) {
        t->token_type = T_IF;

    } else if(!strcmp(str->ptr, "else")) {
        t->token_type = T_ELSE;

    } else if(!strcmp(str->ptr, "do")) {
        t->token_type = T_DO;

    } else if(!strcmp(str->ptr, "end")) {
        t->token_type = T_END;

    } else if(!strcmp(str->ptr, "function")) {
        t->token_type = T_FUNCTION;

    } else if(!strcmp(str->ptr, "global")) {
        t->token_type = T_GLOBAL;

    } else if(!strcmp(str->ptr, "nil")) {
        t->token_type = T_NIL;

    } else if(!strcmp(str->ptr, "require")) {
        t->token_type = T_REQUIRE;

    } else if(!strcmp(str->ptr, "return")) {
        t->token_type = T_RETURN;

    } else if(!strcmp(str->ptr, "while")) {
        t->token_type = T_WHILE;

    } else if(!strcmp(str->ptr, "then")) {
        t->token_type = T_THEN;

    } else if(!strcmp(str->ptr, "integer")) {
        t->token_type = T_TYPE;
        t->type = TYPE_INTEGER;

    } else if(!strcmp(str->ptr, "number")) {
        t->token_type = T_TYPE;
        t->type = TYPE_NUMBER;

    } else if(!strcmp(str->ptr, "local")) {
        t->token_type = T_LOCAL;

    } else if(!strcmp(str->ptr, "string")) {
        t->token_type = T_TYPE;
        t->type = TYPE_STRING;

    } else if(!strcmp(str->ptr, "boolean")) {
        t->token_type = T_TYPE;
        t->type = TYPE_BOOL;

    } else if(!strcmp(str->ptr, "true")) {
        t->token_type = T_BOOL;
        t->boolean = true;

    } else if(!strcmp(str->ptr, "false")) {
        t->token_type = T_BOOL;
        t->boolean = false;

    } else if(!strcmp(str->ptr, "elseif")) {
        t->token_type = T_ELSEIF;

    } else if(!strcmp(str->ptr, "repeat")) {
        t->token_type = T_REPEAT;

    } else if(!strcmp(str->ptr, "until")) {
        t->token_type = T_UNTIL;

    } else if(!strcmp(str->ptr, "break")) {
        t->token_type = T_BREAK;

    } else if(!strcmp(str->ptr, "not")) {
        t->token_type = T_NOT;

    } else if(!strcmp(str->ptr, "and")) {
        t->token_type = T_AND;

    } else if(!strcmp(str->ptr, "or")) {
        t->token_type = T_OR;
    } else if(!strcmp(str->ptr, "for")) {
        t->token_type = T_FOR;
    } else {
        t->token_type = T_IDENTIFIER;
        t->string = *str;
        return;
    }
    str_free(str);
}

/**
 * Parses dynamic string to integer.
 *
 *
 * @param[out] t Pointer to string containing integer number.
 * @return 0 on success, otherwise 1.
 */
static int process_integer(string_t *str, token_t *t)
{
    char *p = NULL;
    int64_t int_val = (int64_t) strtol(str->ptr, &p, 10);

    if(*p) {
        return 1;
    } else {
        t->token_type = T_INTEGER;
        t->integer = int_val;
        return 0;
    }
}

/**
 * Parses escape sequence to integer.
 *
 *
 * @param str Pointer to string containing integer number.
 * @return escaped integer on success, otherwise 0.
 */
uint16_t process_escape(char *str)
{
    char *p = NULL;
    unsigned short int_val = (int64_t) strtol(str, &p, 10);

    if(*p) {
        return 0;
    } else {
        if(int_val >= 1 && int_val <= 255) {
            return int_val;
        } else {
            return 0;
        }
    }
}

/**
 * Parses dynamic string to decimal.
 *
 *
 * @param[out] t Pointer to token, to which attributes are assigned
 * @param str Pointer to string containing integer number.
 * @return 0 on success, otherwise return errno.
 */
static int process_decimal(string_t *str, token_t *t)
{
    char *p = NULL;
    double double_val = strtod(str->ptr, &p);

    if(*p) {
        return 1;
    } else {
        t->token_type = T_NUMBER;
        t->number = double_val;
        return 0;
    }
}

void scanner_init(FILE *source_file)
{
    fptr = source_file;
    last_token_ix = 0;
    row = 1;
    column = 0;
}

int scanner_free(void)
{
    if(fclose(fptr)) {
        return E_INT;
    }
    return E_OK;
}

static int _get_next_token(token_t *t)
{
    // Checks if the file to be read is present
    if(!fptr) {
        return E_INT;
    }

    string_t str;

    // Initialises a null terminated empty string
    if(str_create_empty(&str)) {
        return E_INT;
    }

    static int state = SCANNER_STATE_START;
    int c;
    char esc_mem[4] = { 0, 0, 0, '\0' };

    while(true) {

        c = fgetc(fptr);
        column++;

        switch(state) {
        case SCANNER_STATE_START:
            t->row = row;
            t->column = column;
            if(isspace(c)) {

                state = SCANNER_STATE_START;

                if(c == '\n') {
                    row++;
                    column = 0;
                }

            } else if(isdigit(c)) {

                state = SCANNER_STATE_NUMBER;

                if(str_append_char(&str, c)) {
                    str_free(&str);
                    return E_INT;
                }

            } else if(c == '<') {

                state = SCANNER_STATE_LESS_THAN;

            } else if(c == '>') {

                state = SCANNER_STATE_GREATER_THAN;

            } else if(c == '.') {

                state = SCANNER_STATE_DOT;

            } else if(c == '=') {

                state = SCANNER_STATE_EQUALS;

            } else if(c == '~') {

                state = SCANNER_STATE_TILDE;

            } else if(c == '%') {

                str_free(&str);
                t->token_type = T_PERCENT;
                return E_OK;

            } else if(c == '^') {

                str_free(&str);
                t->token_type = T_CARET;
                return E_OK;

            } else if(c == '+') {

                str_free(&str);
                t->token_type = T_PLUS;
                return E_OK;

            } else if(c == '*') {

                str_free(&str);
                t->token_type = T_ASTERISK;
                return E_OK;

            } else if(c == '(') {

                str_free(&str);
                t->token_type = T_LPAREN;
                return E_OK;

            } else if(c == ')') {

                str_free(&str);
                t->token_type = T_RPAREN;
                return E_OK;

            } else if(c == ':') {

                str_free(&str);
                t->token_type = T_COLON;
                return E_OK;

            } else if(c == ',') {

                str_free(&str);
                t->token_type = T_COMMA;
                return E_OK;

            } else if(c == '#') {

                str_free(&str);
                t->token_type = T_HASH;
                return E_OK;

            } else if(c == '/') {

                state = SCANNER_STATE_SLASH;
                str_free(&str);

            } else if(c == '-') {

                state = SCANNER_STATE_COMMENT_DASH_1;

                if(str_append_char(&str, c)) {
                    str_free(&str);
                    return E_INT;
                }

            } else if(c == '"') {

                state = SCANNER_STATE_STRING;

            } else if(c == EOF) {

                t->token_type = T_EOF;
                str_free(&str);
                return E_OK;

            } else if(isalpha(c) || c == '_') {

                state = SCANNER_STATE_KEYWORD_IDENTIFIER;

                if(str_append_char(&str, c)) {
                    str_free(&str);
                    return E_INT;
                }
            } else {
                str_free(&str);
                return E_LEX;
            }

            break;

        case SCANNER_STATE_NUMBER:
            if(isdigit(c)) {

                if(str_append_char(&str, c)) {
                    str_free(&str);
                    return E_INT;
                }

            } else if(c == '.') {

                state = SCANNER_STATE_DECIMAL;

                if(str_append_char(&str, c)) {
                    str_free(&str);
                    return E_INT;
                }

            } else if(tolower(c) == 'e') {
                state = SCANNER_STATE_EXPONENT;

                if(str_append_char(&str, c)) {
                    str_free(&str);
                    return E_INT;
                }

            } else {

                ungetc(c, fptr);
                column--;

                state = SCANNER_STATE_START;

                if(process_integer(&str, t)) {
                    str_free(&str);
                    return E_INT;
                } else {
                    str_free(&str);
                    return E_OK;
                }
            }
            break;
        case SCANNER_STATE_DECIMAL:
            if(isdigit(c)) {
                if(str_append_char(&str, c)) {
                    str_free(&str);
                    return E_INT;
                }
            } else if(tolower(c) == 'e') {

                state = SCANNER_STATE_EXPONENT;

                if(str_append_char(&str, tolower(c))) {
                    str_free(&str);
                    return E_INT;
                }

            } else {
                ungetc(c, fptr);
                column--;

                state = SCANNER_STATE_START;

                if(process_decimal(&str, t)) {
                    str_free(&str);
                    return E_INT;
                } else {
                    str_free(&str);
                    return E_OK;
                }
            }
            break;
        case SCANNER_STATE_EXPONENT:
            if(c == '+' || c == '-') {

                state = SCANNER_STATE_EXPONENT_VALUE;

                if(str_append_char(&str, c)) {
                    str_free(&str);
                    return E_INT;
                }
            } else if(isdigit(c)) {

                state = SCANNER_STATE_EXPONENT_VALUE;

                if(str_append_char(&str, c)) {
                    str_free(&str);
                    return E_INT;
                }
            } else {

                str_free(&str);
                return E_LEX;
            }
            break;
        case SCANNER_STATE_EXPONENT_VALUE:
            if(isdigit(c)) {
                if(str_append_char(&str, c)) {
                    str_free(&str);
                    return E_INT;
                }

            } else {

                ungetc(c, fptr);
                column--;

                state = SCANNER_STATE_START;

                if(process_decimal(&str, t)) {
                    str_free(&str);
                    return E_INT;
                } else {
                    str_free(&str);
                    return E_OK;
                }
            }
            break;
        case SCANNER_STATE_DOT:

            state = SCANNER_STATE_START;
            str_free(&str);

            if(c == '.') {
                t->token_type = T_DOUBLE_DOT;
                return E_OK;
            } else {
                return E_LEX;
            }
            break;

        case SCANNER_STATE_LESS_THAN:

            state = SCANNER_STATE_START;
            str_free(&str);

            if(c == '=') {
                t->token_type = T_LTE;
                return E_OK;
            } else {
                ungetc(c, fptr);
                column--;
                t->token_type = T_LT;
                return E_OK;
            }
            break;

        case SCANNER_STATE_GREATER_THAN:

            state = SCANNER_STATE_START;
            str_free(&str);

            if(c == '=') {
                t->token_type = T_GTE;
                return E_OK;
            } else {
                ungetc(c, fptr);
                column--;
                t->token_type = T_GT;
                return E_OK;
            }
            break;

        case SCANNER_STATE_EQUALS:

            state = SCANNER_STATE_START;
            str_free(&str);

            if(c == '=') {

                t->token_type = T_DOUBLE_EQUALS;
                return E_OK;
            } else {
                ungetc(c, fptr);
                column--;
                t->token_type = T_EQUALS;
                return E_OK;
            }
            break;

        case SCANNER_STATE_TILDE:

            state = SCANNER_STATE_START;
            str_free(&str);

            if(c == '=') {
                t->token_type = T_TILDE_EQUALS;
                return E_OK;
            } else {
                ungetc(c, fptr);
                column--;
                return E_LEX;
            }
            break;

        case SCANNER_STATE_SLASH:

            state = SCANNER_STATE_START;

            if(c == '/') {
                t->token_type = T_DOUBLE_SLASH;
                return E_OK;
            } else {
                ungetc(c, fptr);
                column--;
                t->token_type = T_SLASH;
                return E_OK;
            }

        case SCANNER_STATE_COMMENT_DASH_1:

            if(c == '-') {

                state = SCANNER_STATE_COMMENT_DASH_2;

            } else {
                ungetc(c, fptr);
                column--;
                str_free(&str);
                state = SCANNER_STATE_START;
                t->token_type = T_MINUS;
                return E_OK;
            }
            break;

        case SCANNER_STATE_COMMENT_DASH_2:

            if(c == '[') {
                state = SCANNER_STATE_BLOCK_COMMENT_BRACKET;
            } else {
                state = SCANNER_STATE_INLINE_COMMENT;
            }
            break;

        case SCANNER_STATE_INLINE_COMMENT:

            if(c == '\n') {
                state = SCANNER_STATE_START;
                row++;
                column = 0;

                str_free(&str);
                str_create_empty(&str);
            } else if(c == EOF) {
                str_free(&str);
                return E_LEX;
            }
            break;

        case SCANNER_STATE_BLOCK_COMMENT_BRACKET:

            if(c == '[') {
                state = SCANNER_STATE_BLOCK_COMMENT;

            } else {
                state = SCANNER_STATE_INLINE_COMMENT;
            }
            break;

        case SCANNER_STATE_BLOCK_COMMENT:

            if(c == ']') {
                state = SCANNER_STATE_CLOSING_BRACKET;
            } else if(c == '\n') {
                row++;
                column = 0;
            } else if(c == EOF) {
                str_free(&str);
                return E_LEX;
            }

            break;
        case SCANNER_STATE_CLOSING_BRACKET:

            if(c == ']') {
                state = SCANNER_STATE_START;

                str_free(&str);
                if(str_create_empty(&str)) {
                    return E_INT;
                }

            } else if(c == EOF) {
                str_free(&str);
                return E_LEX;
            }

            break;
        case SCANNER_STATE_STRING:

            if(c == '\\') {

                state = SCANNER_STATE_ESCAPE_CHAR_SEQ;

            } else if(c == '"') {
                state = SCANNER_STATE_START;
                t->token_type = T_STRING;
                t->string = str;

                return E_OK;

            } else if(c == EOF) {

                str_free(&str);
                return E_LEX;

            } else {
                if(c == '\n') {
                    row++;
                    column = 0;
                }

                if(str_append_char(&str, c)) {
                    str_free(&str);
                    return E_INT;
                }
            }
            break;

        case SCANNER_STATE_ESCAPE_CHAR_SEQ:

            if(c == '\\') {

                state = SCANNER_STATE_STRING;

                if(str_append_char(&str, c)) {
                    str_free(&str);
                    return E_INT;
                }
            } else if(c == 'n') {
                state = SCANNER_STATE_STRING;

                if(str_append_char(&str, '\n')) {
                    str_free(&str);
                    return E_INT;
                }
            } else if(c == 't') {
                state = SCANNER_STATE_STRING;

                if(str_append_char(&str, '\t')) {
                    str_free(&str);
                    return E_INT;
                }
            } else if(c == '"') {
                state = SCANNER_STATE_STRING;

                if(str_append_char(&str, '"')) {
                    str_free(&str);
                    return E_INT;
                }
            } else if(isdigit(c)) {
                state = SCANNER_STATE_ESCAPE_SEQ_1;

                esc_mem[0] = c;

            } else {
                str_free(&str);
                return E_LEX;
            }
            break;
        case SCANNER_STATE_ESCAPE_SEQ_1:
            if(isdigit(c)) {
                state = SCANNER_STATE_ESCAPE_SEQ_2;

                esc_mem[1] = c;
            } else {
                str_free(&str);
                return E_LEX;
            }
            break;

        case SCANNER_STATE_ESCAPE_SEQ_2:
            if(isdigit(c)) {
                esc_mem[2] = c;

                short unsigned int val = process_escape(esc_mem);
                if(val) {
                    if(str_append_char(&str, val)) {
                        str_free(&str);
                        return E_INT;
                    }
                } else {
                    str_free(&str);
                    return E_LEX;
                }
                state = SCANNER_STATE_STRING;
            }
            break;
        case SCANNER_STATE_KEYWORD_IDENTIFIER:
            if(c == '_' || isdigit(c) || isalpha(c)) {
                if(str_append_char(&str, c)) {
                    str_free(&str);
                    return E_INT;
                }
            } else {

                state = SCANNER_STATE_START;

                ungetc(c, fptr);
                column--;
                identify_keyword(&str, t);
                return E_OK;
            }
        }
    }
}

int get_next_token(token_t *t)
{
    if(last_token_ix) {
        *t = last_tokens[--last_token_ix];
        return E_OK;
    }
    int ret = _get_next_token(t);
    for(int i = TOKEN_BUF_LENGTH - 1; i > 0; i--) {
        last_tokens[i] = last_tokens[i - 1];
    }
    last_tokens[0] = *t;
    return ret;
}

int unget_token()
{
    // can't unget any more tokens, buffer is full
    if(last_token_ix >= TOKEN_BUF_LENGTH) {
        return E_LEX;
    }
    last_token_ix++;
    return E_OK;
}
