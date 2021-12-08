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
 * @file codegen.c
 *
 * @brief IFJCode21 generator from an abstract syntax tree
 */

#include <stdio.h>
#include "error.h"
#include "ast.h"
#include <string.h>
#include <ctype.h>
#include "hashtable_bst.h"
#include "semantics.h"
#include "optimizations.h"
#include "stack.h"

static bool comments = false;

#define OUTPUT_COMMENT(...)                                                                        \
    if(comments) {                                                                                 \
        printf("# ");                                                                              \
        printf(__VA_ARGS__);                                                                       \
    }

#define OUTPUT_CODE(...) printf(__VA_ARGS__)

#define OUTPUT_CODE_LINE(code) printf("%s\n", code)

#define OUTPUT_CODE_PART(code) printf("%s", code)

#define EMPTY_LINE printf("\n")

#define COMMENT(comm) printf("#%s\n", comm)

// Codegen initialization
void avengers_assembler(ast_node_t *ast);

void generate_header();

void process_node_program(ast_node_t *cur_node);

// All IFJcode21 premade code.
void generate_builtin();

// Builtin functions
void generate_reads();

void generate_readi();

void generate_readn();

void generate_tointeger();

void generate_chr();

void generate_ord();

void generate_substring();

// Additional builtin functions
void int_zerodivcheck();

void float_zerodivcheck();

void nil_check();

void check_for_conversion();

void conv_to_float();

void check_if_int();

void exponentiation();

void exponent_float_to_integer();

void should_i_jump();

void zero_step();

void for_convert();

void eval_condition();

void check_nil_write();

// Main switch
void process_node(ast_node_t *cur_node, int break_label);

// Following processing functions
void process_unop_node(ast_node_t *unop_node);

void process_binop_node(ast_node_t *binop_node);

void process_declaration_node(ast_node_t *cur_node, bool is_in_loop);

void process_assignment_node(ast_node_t *cur_node);

void process_node_func_call(ast_node_t *cur_node);

void process_for_node(ast_node_t *for_node);

void generate_if_code(ast_node_t *condition, ast_node_t *body, int local_label_counter,
                      int break_label);

void process_if_node(ast_node_t *cur_node, int break_label);

void process_while_node(ast_node_t *cur_node);

void process_repeat_until(ast_node_t *cur_node);

void process_node_func_def(ast_node_t *cur_node);

void process_return_node(ast_node_t *return_node);

// Additional helping functions
void process_string(char *s);

void look_for_declarations(ast_node_t *root);

void output_label(int label_counter);

void generate_result();

int count_children(ast_node_list_t children_list);

void push_integer_arg(uint64_t integer);

void push_number_arg(double number);

void push_bool_arg(bool boolean);

void push_string_arg(char *string);

void push_id_arg(symbol_t *symbol);

void push_nil_arg();

void generate_write(int arg_count);

void generate_func_start(char *function_name);

void generate_func_arg(symbol_t *symbol, int i);

void generate_func_retval_dec(int i);

void generate_func_call_assignment_RL(ast_node_t *rvalue, int lside_counter);

int generate_func_call_assignment(ast_node_t *rvalue, int lside_counter);

void generate_integer_push(ast_node_t *rvalue);

void generate_symbol_push(ast_node_t *rvalue);

void generate_number_push(ast_node_t *rvalue);

void generate_bool_push(ast_node_t *rvalue);

void generate_string_push(ast_node_t *rvalue);

void generate_nil_push();

void ret_integer_arg(uint64_t integer);

void ret_number_arg(double number);

void ret_bool_arg(bool boolean);

void ret_string_arg(char *string);

void ret_id_arg(symbol_t *symbol);

void ret_nil_arg();

void generate_func_def_retval_assign(int i);

void ret_binop_arg();

void generate_binop_assignment(ast_node_t *rvalue);

void generate_unop_assignment(ast_node_t *rvalue);

void generate_result();

void process_return_node(ast_node_t *return_node);

void generate_declaration(symbol_t *symbol);

void generate_move(symbol_t *symbol);

void look_for_declarations(ast_node_t *root);

// Assignments

void generate_integer_assignment(ast_node_t *rvalue);

void generate_id_assignment(ast_node_t *rvalue);

void generate_number_assignment(ast_node_t *rvalue);

void generate_bool_assignment(ast_node_t *rvalue);

void generate_string_assignment(ast_node_t *rvalue);

void generate_nil_assignment();

void generate_func_call_assignment_decl(ast_node_t *rvalue);

static uint64_t hash(const char *key)
{
    uint64_t h = 0;
    for(const char *c = key; *c != '\0'; ++c) {
        h = ((h << 5) ^ (h >> 27)) ^ *c;
    }
    return h;
}

void exponent_float_to_integer()
{
    OUTPUT_CODE_LINE("LABEL FLOAT_TO_INT_EXPONENT");
    OUTPUT_CODE_LINE("POPS GF@result");

    OUTPUT_CODE_LINE("PUSHS GF@RESULT");
    OUTPUT_CODE_LINE("RETURN");
}

int global_label_counter = 0;

void output_label(int label_counter)
{
    printf("%%%d", label_counter);
}

void process_string(char *s)
{
    int i = 0;
    while(s[i] != '\0') {
        if(s[i] <= 32) {
            printf("\\%03d", (uint8_t) s[i]);
        } else if(s[i] == '#') {
            printf("\\035");
        } else if(s[i] == '\\') {
            printf("\\092");
        } else {
            printf("%c", s[i]);
        }
        i++;
    }
    printf("\n");
}

static int global_func_counter = 0;

void process_binop_node(ast_node_t *binop_node);
void process_unop_node(ast_node_t *unop_node);
void process_node(ast_node_t *cur_node, int break_label);
void generate_result();

char *get_symbol_name(symbol_t *node_symbol)
{
    if(node_symbol->is_declaration) {
        return node_symbol->name.ptr;
    } else {
        return get_symbol_name(node_symbol->declaration);
    }
}

void print_symbol(symbol_t *symbol)
{
    printf("%s", get_symbol_name(symbol));
}

int count_children(ast_node_list_t children_list)
{
    ast_node_t *first = children_list;
    int counter = 0;
    while(first != NULL) {
        counter++;
        first = first->next;
    }
    return counter;
}

void push_integer_arg(uint64_t integer)
{
    printf("int@%ld\n", integer);
}

void push_number_arg(double number)
{
    printf("float@%a\n", number);
}

void push_bool_arg(bool boolean)
{
    if(boolean == 1) {
        printf("bool@true\n");
    } else {
        printf("bool@false\n");
    }
}

void push_string_arg(char *string)
{
    OUTPUT_CODE_PART("string@");
    process_string(string);
    OUTPUT_CODE_LINE("");
}

void push_id_arg(symbol_t *symbol)
{
    printf("LF@%s\n", get_symbol_name(symbol));
}
void push_nil_arg()
{
    printf("nil@nil\n");
}

void check_nil_write()
{
    OUTPUT_CODE_LINE("LABEL nil_write");
    OUTPUT_CODE_LINE("POPS GF@op1");
    OUTPUT_CODE_LINE("TYPE GF@type1 GF@op1");
    OUTPUT_CODE_LINE("JUMPIFEQ IS_NIL string@nil GF@type1");
    OUTPUT_CODE_LINE("WRITE GF@op1");
    OUTPUT_CODE_LINE("JUMP END_WRITE");
    OUTPUT_CODE_LINE("LABEL IS_NIL");
    OUTPUT_CODE_LINE("WRITE string@nil");
    OUTPUT_CODE_LINE("LABEL END_WRITE");
    OUTPUT_CODE_LINE("PUSHS GF@op1");
    OUTPUT_CODE_LINE("RETURN");
}

void generate_write(int arg_count)
{
    for(int i = 0; i < arg_count; i++) {
        OUTPUT_CODE_PART("PUSHS TF@%");
        printf("%d\n", i);
        OUTPUT_CODE_LINE("CALL nil_write");
        OUTPUT_CODE_PART("POPS TF@%");
        printf("%d\n", i);
    }
}

void generate_func_start(char *function_name)
{
    OUTPUT_CODE_PART("LABEL $");
    OUTPUT_CODE_LINE(function_name);
    OUTPUT_CODE_LINE("PUSHFRAME");
}

void generate_func_arg(symbol_t *symbol, int i)
{
    char *id = get_symbol_name(symbol);
    OUTPUT_CODE_PART("DEFVAR LF@");
    printf("%s\n", id);
    OUTPUT_CODE_PART("MOVE LF@");
    printf("%s ", id);
    OUTPUT_CODE_PART("LF@%");
    printf("%d\n", i);
}

void generate_func_retval_dec(int i)
{
    OUTPUT_CODE_PART("DEFVAR LF@retval");
    printf("%d\n", i);
    OUTPUT_CODE_PART("MOVE LF@retval");
    printf("%d ", i);
    OUTPUT_CODE_LINE("nil@nil");
}

hashtable_t declarations;

void process_node_func_def(ast_node_t *cur_node)
{
    generate_func_start(cur_node->func_def.name.ptr);

    ast_node_t *arg = cur_node->func_def.arguments;
    int arg_counter = 0;
    while(arg != NULL) {
        generate_func_arg(&arg->symbol, arg_counter);
        arg = arg->next;
        arg_counter++;
    }

    int retval_counter = 0;
    ast_node_t *retval_type = cur_node->func_def.return_types;
    while(retval_type != NULL) {
        generate_func_retval_dec(retval_counter);
        retval_type = retval_type->next;
        retval_counter++;
    }

    hashtable_create_bst(&declarations, 47, hash);
    look_for_declarations(cur_node->func_def.body);
    hashtable_free(&declarations);
    process_node(cur_node->func_def.body, 0);
    global_func_counter++;
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
    EMPTY_LINE;
}

void generate_func_call_assignment_RL(ast_node_t *rvalue, int lside_counter)
{
    process_node_func_call(rvalue);

    if(rvalue->next) { // If the func call is not the last in assignment right side, only the first
                       // retval is used.
        OUTPUT_CODE_LINE("PUSHS TF@retval0");
    }

    if(rvalue->next == NULL) { // We can return more than one value if the last item in list is
                               // function and pad with nil if an argument is missing.

        int ret_count;
        if(rvalue->func_call.def) {
            ret_count = count_children(rvalue->func_call.def->return_types);
        } else {
            ret_count = count_children(rvalue->func_call.decl->return_types);
        }

        for(int i = 0; i < lside_counter; i++) {
            OUTPUT_CODE_PART("PUSHS TF@retval");
            printf("%d\n", i); // Push all children.
        }

        for(int k = 0; k < lside_counter - ret_count; k++) { // If need be, pad with nils
            OUTPUT_CODE_LINE("PUSHS nil@nil");
        }
    }
}

int generate_func_call_assignment(ast_node_t *rvalue, int lside_counter)
{
    process_node_func_call(rvalue);
    if(rvalue->next) { // If the func call is not the last in assignment right side, only the
                       // first retval is used.
        OUTPUT_CODE_LINE("PUSHS TF@retval0");
        return 0;
    }

    else { // We can return more than one value if the last item in list is
           // function and pad with nil if an argument is missing.
        int ret_count;
        if(rvalue->func_call.def) {
            ret_count = count_children(rvalue->func_call.def->return_types);
        } else {
            ret_count = count_children(rvalue->func_call.decl->return_types);
        }

        for(int i = 0; i < ret_count; i++) {
            OUTPUT_CODE_PART("PUSHS TF@retval");
            printf("%d\n", i); // Push all children.
        }

        for(int k = 0; k < lside_counter - ret_count; k++) { // If need be, pad with nils
            OUTPUT_CODE_LINE("PUSHS nil@nil");
        }
        return ret_count;
    }
}

void generate_integer_push(ast_node_t *rvalue)
{
    printf("int@%ld\n", rvalue->integer);
}

void generate_symbol_push(ast_node_t *rvalue)
{
    printf("LF@%s\n", get_symbol_name(&rvalue->symbol));
}

void generate_number_push(ast_node_t *rvalue)
{
    printf("float@%a\n", rvalue->number);
}

void generate_bool_push(ast_node_t *rvalue)
{
    if(rvalue->boolean == 1) {
        printf("bool@true\n");
    } else {
        printf("bool@false\n");
    }
}

void generate_string_push(ast_node_t *rvalue)
{
    OUTPUT_CODE_PART("string@");
    process_string(rvalue->string.ptr);
    OUTPUT_CODE_LINE("");
}

void generate_nil_push()
{
    printf("nil@nil\n");
}

void ret_integer_arg(uint64_t integer)
{
    printf("int@%ld\n", integer);
}

void ret_number_arg(double number)
{
    printf("float@%a\n", number);
}

void ret_bool_arg(bool boolean)
{
    if(boolean == 1) {
        printf("bool@true\n");
    } else {
        printf("bool@false\n");
    }
}

void ret_string_arg(char *string)
{
    OUTPUT_CODE_PART("string@");
    process_string(string);
    OUTPUT_CODE_LINE("");
}

void ret_id_arg(symbol_t *symbol)
{
    printf("LF@%s\n", get_symbol_name(symbol));
}

void ret_nil_arg()
{
    printf("nil@nil\n");
}

void generate_func_def_retval_assign(int i)
{
    OUTPUT_CODE_PART("MOVE LF@retval");
    printf("%d ", i);
}

void ret_binop_arg()
{
    OUTPUT_CODE_LINE("POPS GF@result");
}

void generate_binop_assignment(ast_node_t *rvalue)
{
    process_binop_node(rvalue);
    OUTPUT_CODE_LINE("POPS GF@result");
}

void generate_unop_assignment(ast_node_t *rvalue)
{
    process_unop_node(rvalue);
    OUTPUT_CODE_LINE("POPS GF@result");
}

bool can_be_nil(ast_node_t *node)
{
    bool nil_check = !optimus_prime;
    if(!nil_check) {
        switch(node->node_type) {
        case AST_NODE_BINOP:
            if(!is_not_nil(node->binop.left)) {
                nil_check = true;
                break;
            }
            if(!is_not_nil(node->binop.right)) {
                nil_check = true;
                break;
            }
            break;
        case AST_NODE_UNOP:
            if(!is_not_nil(node->unop.operand)) {
                nil_check = true;
                break;
            }
            break;
        default:
            if(!is_not_nil(node)) {
                nil_check = true;
                break;
            }
            break;
        }
    }
    return nil_check;
}

bool needs_conversion(ast_node_t *node)
{
    switch(node->node_type) {
    case AST_NODE_BINOP: {
        type_t optype;
        if(sem_get_type(node, &optype) != E_OK) {
            return true;
        }
        if(optype == TYPE_NUMBER) {
            type_t left;
            if(sem_get_type(node->binop.left, &left) != E_OK) {
                return true;
            }
            if(left == TYPE_INTEGER) {
                return true;
            }
            type_t right;
            if(sem_get_type(node->binop.right, &right) != E_OK) {
                return true;
            }
            if(right == TYPE_INTEGER) {
                return true;
            }
            return false;
        }
    } break;
    case AST_NODE_UNOP: {
        type_t optype;
        if(sem_get_type(node, &optype) != E_OK) {
            return true;
        }
        if(optype == TYPE_NUMBER) {
            type_t type;
            if(sem_get_type(node->unop.operand, &type) != E_OK) {
                return true;
            }
            if(type == TYPE_INTEGER) {
                return true;
            }
            return false;
        }
    } break;
    default:
        break;
    }
    return true;
}

void output_conv_check(ast_node_t *node)
{
    if(needs_conversion(node)) {
        OUTPUT_CODE_LINE("CALL CONV_CHECK");
    }
}

void output_nil_check(ast_node_t *node)
{
    if(can_be_nil(node)) {
        OUTPUT_CODE_LINE("CALL NIL_CHECK");
    }
}

void process_unop_node(ast_node_t *unop_node)
{
    switch(unop_node->unop.type) {
    case AST_NODE_UNOP_LEN:
        process_binop_node(unop_node->unop.operand);
        if(can_be_nil(unop_node)) {
            OUTPUT_CODE_LINE("POPS GF@result");
            OUTPUT_CODE_LINE("JUMPIFEQ NIL_FOUND GF@result nil@nil");
            OUTPUT_CODE_LINE("STRLEN GF@result GF@result");
            OUTPUT_CODE_LINE("PUSHS GF@result");
        }
        break;
    case AST_NODE_UNOP_NOT:
        process_binop_node(unop_node->unop.operand);
        OUTPUT_CODE_LINE("PUSHS int@2");
        output_nil_check(unop_node);
        // OUTPUT_CODE_LINE("CALL NIL_CHECK");
        OUTPUT_CODE_LINE("POPS GF@trash");
        OUTPUT_CODE_LINE("NOTS");
        break;
    case AST_NODE_UNOP_NEG:
        process_binop_node(unop_node->unop.operand);
        OUTPUT_CODE_LINE("PUSHS int@-1");
        // OUTPUT_CODE_LINE("CALL NIL_CHECK");
        output_nil_check(unop_node);
        output_conv_check(unop_node);
        OUTPUT_CODE_LINE("MULS");
        break;
    default:
        break;
    }
}
// binop_node->binop.type == AST_NODE_BINOP_OR
void process_binop_node(ast_node_t *binop_node)
{
    global_label_counter++;
    int local_label_counter = global_label_counter;
    global_label_counter++;
    int second_local_label_counter = global_label_counter;
    if(binop_node->node_type == AST_NODE_BINOP) {
        process_binop_node(binop_node->binop.left);

        if(binop_node->binop.type ==
           AST_NODE_BINOP_OR) { // Pri OR skaceme na koniec, ak bola prva cast true.
            OUTPUT_CODE_LINE("POPS GF@result");
            OUTPUT_CODE_LINE("PUSHS GF@result");
            OUTPUT_CODE_PART("JUMPIFEQ ");
            output_label(local_label_counter);
            OUTPUT_CODE_LINE(" GF@result bool@true");
        }
        if(binop_node->binop.type ==
           AST_NODE_BINOP_AND) { // Pri AND skaceme na koniec, ak bola prva cast false.
            OUTPUT_CODE_LINE("POPS GF@result");
            OUTPUT_CODE_LINE("PUSHS GF@result");
            OUTPUT_CODE_PART("JUMPIFEQ ");
            output_label(local_label_counter);
            OUTPUT_CODE_LINE(" GF@result bool@false");
        }
        process_binop_node(binop_node->binop.right);
        OUTPUT_CODE_PART("JUMP ");
        output_label(second_local_label_counter);
        printf("\n");
        OUTPUT_CODE_PART("LABEL ");
        output_label(local_label_counter);
        printf("\n");

        if(binop_node->binop.type ==
           AST_NODE_BINOP_OR) { // Prva cast oru bola true, pridame este jedno true.
            OUTPUT_CODE_LINE("PUSHS bool@true");
        }
        if(binop_node->binop.type ==
           AST_NODE_BINOP_AND) { // Prva cast andu bola false, pridame este jedno false.
            OUTPUT_CODE_LINE("PUSHS bool@false");
        }
        OUTPUT_CODE_PART("LABEL ");
        output_label(second_local_label_counter);
        printf("\n");
    } else {
        switch(binop_node->node_type) {
        case AST_NODE_UNOP:
            process_unop_node(binop_node);
            break;
        case AST_NODE_INTEGER:
            OUTPUT_CODE_PART("PUSHS ");
            ret_integer_arg(binop_node->integer);
            break;
        case AST_NODE_NUMBER:
            OUTPUT_CODE_PART("PUSHS ");
            ret_number_arg(binop_node->number);
            break;
        case AST_NODE_BOOLEAN:
            OUTPUT_CODE_PART("PUSHS ");
            ret_bool_arg(binop_node->boolean);
            break;
        case AST_NODE_SYMBOL:
            OUTPUT_CODE_PART("PUSHS ");
            ret_id_arg(&(binop_node->symbol));
            break;
        case AST_NODE_STRING:
            OUTPUT_CODE_PART("PUSHS ");
            ret_string_arg(binop_node->string.ptr);
            break;
        case AST_NODE_NIL:
            OUTPUT_CODE_PART("PUSHS ");
            ret_nil_arg();
            break;
        case AST_NODE_FUNC_CALL:
            process_node_func_call(binop_node);
            OUTPUT_CODE_LINE("PUSHS TF@retval0");
            break;
        default:
            break;
        }
        return;
    }
    switch(binop_node->binop.type) {
    case AST_NODE_BINOP_ADD:
        output_nil_check(binop_node);
        output_conv_check(binop_node);
        // OUTPUT_CODE_LINE("CALL NIL_CHECK");
        // OUTPUT_CODE_LINE("CALL CONV_CHECK");
        OUTPUT_CODE_LINE("ADDS");
        break;
    case AST_NODE_BINOP_SUB:
        // OUTPUT_CODE_LINE("CALL NIL_CHECK");
        output_nil_check(binop_node);
        output_conv_check(binop_node);
        // OUTPUT_CODE_LINE("CALL CONV_CHECK");
        OUTPUT_CODE_LINE("SUBS");
        break;
    case AST_NODE_BINOP_MUL:
        // OUTPUT_CODE_LINE("CALL NIL_CHECK");
        output_nil_check(binop_node);
        output_conv_check(binop_node);
        // OUTPUT_CODE_LINE("CALL CONV_CHECK");
        OUTPUT_CODE_LINE("MULS");
        break;
    case AST_NODE_BINOP_DIV:
        // OUTPUT_CODE_LINE("CALL NIL_CHECK");
        output_nil_check(binop_node);
        OUTPUT_CODE_LINE("CALL CONV_TO_FLOAT");
        OUTPUT_CODE_LINE("CALL float_zerodivcheck");
        OUTPUT_CODE_LINE("DIVS");
        break;
    case AST_NODE_BINOP_INTDIV:
        // OUTPUT_CODE_LINE("CALL NIL_CHECK");
        output_nil_check(binop_node);
        OUTPUT_CODE_LINE("CALL CHECK_IF_INT");
        OUTPUT_CODE_LINE("CALL int_zerodivcheck");
        OUTPUT_CODE_LINE("IDIVS");
        break;
    case AST_NODE_BINOP_MOD:
        // MOD
        /*
        A%B = E

        A//B = C
        D = C*B
        E = A-D
        E = A - (A//B)*B

        */
        output_nil_check(binop_node);
        // OUTPUT_CODE_LINE("CALL NIL_CHECK");
        OUTPUT_CODE_LINE("CALL CONV_TO_INT");
        OUTPUT_CODE_LINE("CALL int_zerodivcheck");
        OUTPUT_CODE_LINE("POPS GF@op2");
        OUTPUT_CODE_LINE("POPS GF@op1"); // Saving A and B

        OUTPUT_CODE_LINE("PUSHS GF@op1");
        OUTPUT_CODE_LINE("PUSHS GF@op2"); // Pushing them back (but we know their values now)
                                          // Stack top is on the left.
        OUTPUT_CODE_LINE("IDIVS");        // Stack = (A//B)
        OUTPUT_CODE_LINE("PUSHS GF@op2"); // Stack = B, (A//B)
        OUTPUT_CODE_LINE("MULS");         // Stack = B*(A//B)
        OUTPUT_CODE_LINE("POPS GF@op2");  // Stack = --; GF@op2 =  B*(A//B)
        OUTPUT_CODE_LINE("PUSHS GF@op1"); // Stack = A
        OUTPUT_CODE_LINE("PUSHS GF@op2"); // Stack = B*(A//B), A
        OUTPUT_CODE_LINE("SUBS");         // Stack = A-B*(A//B)
        break;
    case AST_NODE_BINOP_POWER:
        OUTPUT_CODE_LINE("CALL EXPONENTIATION");
        break;
    case AST_NODE_BINOP_LT:
        output_nil_check(binop_node);
        // OUTPUT_CODE_LINE("CALL NIL_CHECK");
        // OUTPUT_CODE_LINE("CALL CONV_CHECK");
        output_conv_check(binop_node);
        OUTPUT_CODE_LINE("LTS");
        break;
    case AST_NODE_BINOP_GT:
        output_nil_check(binop_node);
        // OUTPUT_CODE_LINE("CALL NIL_CHECK");
        // OUTPUT_CODE_LINE("CALL CONV_CHECK");
        output_conv_check(binop_node);
        OUTPUT_CODE_LINE("GTS");
        break;
    case AST_NODE_BINOP_LTE:
        output_nil_check(binop_node);
        // OUTPUT_CODE_LINE("CALL NIL_CHECK");
        // OUTPUT_CODE_LINE("CALL CONV_CHECK");
        output_conv_check(binop_node);
        OUTPUT_CODE_LINE("GTS");
        OUTPUT_CODE_LINE("NOTS");
        break;
    case AST_NODE_BINOP_GTE:
        output_nil_check(binop_node);
        // OUTPUT_CODE_LINE("CALL NIL_CHECK");
        // OUTPUT_CODE_LINE("CALL CONV_CHECK");
        output_conv_check(binop_node);
        OUTPUT_CODE_LINE("LTS");
        OUTPUT_CODE_LINE("NOTS");
        break;
    case AST_NODE_BINOP_EQ:
        // OUTPUT_CODE_LINE("CALL CONV_CHECK");
        output_conv_check(binop_node);
        OUTPUT_CODE_LINE("EQS");
        break;
    case AST_NODE_BINOP_NE:
        //    OUTPUT_CODE_LINE("CALL CONV_CHECK");
        output_conv_check(binop_node);
        OUTPUT_CODE_LINE("EQS");
        OUTPUT_CODE_LINE("NOTS");
        break;
    case AST_NODE_BINOP_AND:
        OUTPUT_CODE_LINE("CALL EVAL_CONDITION");
        OUTPUT_CODE_LINE("POPS GF@op1");
        OUTPUT_CODE_LINE("CALL EVAL_CONDITION");
        OUTPUT_CODE_LINE("PUSHS GF@op1");
        OUTPUT_CODE_LINE("ANDS");
        break;
    case AST_NODE_BINOP_OR:
        OUTPUT_CODE_LINE("CALL EVAL_CONDITION");
        OUTPUT_CODE_LINE("POPS GF@op1");
        OUTPUT_CODE_LINE("CALL EVAL_CONDITION");
        OUTPUT_CODE_LINE("PUSHS GF@op1");
        OUTPUT_CODE_LINE("ORS");
        break;
    case AST_NODE_BINOP_CONCAT:
        OUTPUT_CODE_LINE("POPS GF@string1");
        OUTPUT_CODE_LINE("POPS GF@string0");
        OUTPUT_CODE_LINE("CONCAT GF@result GF@string0 GF@string1");
        OUTPUT_CODE_LINE("PUSHS GF@result");
        break;
    default:
        break;
    }
}

void generate_result()
{
    printf("GF@result\n");
}

void process_return_node(ast_node_t *return_node)
{
    int lside_counter = count_children(return_node->return_values.def->return_types);
    int rside_counter = 0;
    int returned_from_function;
    ast_node_t *cur_retval = return_node->return_values.values;
    for(int i = 0; i < lside_counter; i++) {
        if(cur_retval) {
            switch(cur_retval->node_type) {
            case AST_NODE_SYMBOL:
                OUTPUT_CODE_PART("PUSHS ");
                generate_symbol_push(cur_retval);
                break;
            case AST_NODE_INTEGER:
                OUTPUT_CODE_PART("PUSHS ");
                generate_integer_push(cur_retval);
                break;
            case AST_NODE_NUMBER:
                OUTPUT_CODE_PART("PUSHS ");
                generate_number_push(cur_retval);
                break;
            case AST_NODE_BOOLEAN:
                OUTPUT_CODE_PART("PUSHS ");
                generate_bool_push(cur_retval);
                break;
            case AST_NODE_STRING:
                OUTPUT_CODE_PART("PUSHS ");
                generate_string_push(cur_retval);
                break;
            case AST_NODE_NIL:
                OUTPUT_CODE_PART("PUSHS ");
                generate_nil_push();
                break;
            case AST_NODE_FUNC_CALL:
                returned_from_function =
                    generate_func_call_assignment(cur_retval, lside_counter - rside_counter);
                rside_counter = rside_counter + returned_from_function - 1;
                i = i + returned_from_function - 1;
                break;
            case AST_NODE_BINOP:
                generate_binop_assignment(cur_retval);
                OUTPUT_CODE_PART("PUSHS ");
                generate_result();
                break;
            case AST_NODE_UNOP:
                generate_unop_assignment(cur_retval);
                OUTPUT_CODE_PART("PUSHS ");
                generate_result();
                break;
            default:
                break;
            }
            rside_counter++;
            cur_retval = cur_retval->next;
        } else {
            OUTPUT_CODE_LINE("PUSHS nil@nil");
        }
    }

    for(int j = 0; j < rside_counter - lside_counter; j++) {
        OUTPUT_CODE_LINE("POPS GF@trash"); // Losing unwanted expression results.
    }
    for(int l = 0; l < lside_counter; l++) {
        OUTPUT_CODE_LINE("POPS GF@result");
        printf("MOVE LF@retval%d GF@result\n", lside_counter - 1 - l);
    }
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
}

void generate_integer_assignment(ast_node_t *rvalue)
{
    printf("int@%ld\n", rvalue->integer);
}

void generate_id_assignment(ast_node_t *rvalue)
{
    printf("LF@%s\n", get_symbol_name(&rvalue->symbol));
}

void generate_number_assignment(ast_node_t *rvalue)
{
    printf("float@%a\n", rvalue->number);
}

void generate_bool_assignment(ast_node_t *rvalue)
{
    if(rvalue->boolean == 1) {
        printf("bool@true\n");
    } else {
        printf("bool@false\n");
    }
}

void generate_string_assignment(ast_node_t *rvalue)
{
    OUTPUT_CODE_PART("string@");
    process_string(rvalue->string.ptr);
    OUTPUT_CODE_LINE("");
}

void generate_nil_assignment()
{
    printf("nil@nil\n");
}

void generate_func_call_assignment_decl(ast_node_t *rvalue)
{
    process_node_func_call(rvalue);
    OUTPUT_CODE_LINE("MOVE GF@result TF@retval0");
}

void generate_declaration(symbol_t *symbol)
{
    char *id = get_symbol_name(symbol);

    void *garbo = NULL;
    if(hashtable_find(&declarations, id, &garbo) != E_OK) {
        hashtable_insert(&declarations, id, NULL);
        printf("DEFVAR LF@%s\n", id);
    }
}

void generate_move(symbol_t *symbol)
{
    printf("MOVE LF@%s ", get_symbol_name(symbol));
}

void process_declaration_node(ast_node_t *cur_node, bool is_in_loop)
{
    ast_node_t *rvalue = cur_node->declaration.assignment;
    if(is_in_loop) {
        generate_declaration(&cur_node->declaration.symbol);
        return;
    }
    if(!rvalue) {
        generate_move(&cur_node->declaration.symbol);
        generate_nil_assignment();
        return;
    } else {
        switch(rvalue->node_type) {
        case AST_NODE_SYMBOL:
            generate_move(&cur_node->declaration.symbol);
            generate_id_assignment(rvalue);
            break;
        case AST_NODE_INTEGER:
            generate_move(&cur_node->declaration.symbol);
            generate_integer_assignment(rvalue);
            break;
        case AST_NODE_NUMBER:
            generate_move(&cur_node->declaration.symbol);
            generate_number_assignment(rvalue);
            break;
        case AST_NODE_BOOLEAN:
            generate_move(&cur_node->declaration.symbol);
            generate_bool_assignment(rvalue);
            break;
        case AST_NODE_STRING:
            generate_move(&cur_node->declaration.symbol);
            generate_string_assignment(rvalue);
            break;
        case AST_NODE_NIL:
            generate_move(&cur_node->declaration.symbol);
            generate_nil_assignment();
            break;
        case AST_NODE_FUNC_CALL:
            generate_func_call_assignment_decl(rvalue);
            generate_move(&cur_node->declaration.symbol);
            generate_result();
            break;
        case AST_NODE_BINOP:
            generate_binop_assignment(rvalue);
            generate_move(&cur_node->declaration.symbol);
            generate_result();
            break;
        case AST_NODE_UNOP:
            generate_unop_assignment(rvalue);
            generate_move(&cur_node->declaration.symbol);
            generate_result();
            break;
        default:
            break;
        }
    }
}

typedef struct {
    ast_node_t *source;
    ast_node_t *dest;
} assignment_data_t;

void process_assignment_node(ast_node_t *cur_node)
{
    ast_node_t *identifier_iterator = cur_node->assignment.identifiers;
    int lside_counter = 0;
    while(identifier_iterator != NULL) {
        lside_counter++;
        identifier_iterator = identifier_iterator->next;
    }
    int rside_counter = 0;
    ast_node_t *expression_iterator = cur_node->assignment.expressions;
    while(expression_iterator != NULL) {
        rside_counter++;
        expression_iterator = expression_iterator->next;
    }

    adt_stack_t stack;
    if(stack_create(&stack, rside_counter * 2) != E_OK) {
        // todo error
    }

    ast_node_t *expression = cur_node->assignment.expressions;
    ast_node_t *identifier = cur_node->assignment.identifiers;
    while(expression && identifier) {

        switch(expression->node_type) {
        case AST_NODE_SYMBOL:
            //            OUTPUT_CODE_PART("PUSHS ");
            //            generate_symbol_push(expression);
            //            OUTPUT_CODE_PART("POPS LF@");
            //            print_symbol(&);
            //            OUTPUT_CODE_LINE("\n");
            {
                bool push = false;
                for(ast_node_t *it = cur_node->assignment.identifiers; it; it = it->next) {
                    if(strcmp(it->symbol.declaration->name.ptr,
                              identifier->symbol.declaration->name.ptr) == 0) {
                        push = true;
                        break;
                    }
                }
                if(push) {
                    OUTPUT_CODE("PUSHS LF@%s\n", get_symbol_name(&expression->symbol));
                    stack_push(&stack, identifier);
                } else {
                    OUTPUT_CODE("MOVE LF@%s ", get_symbol_name(&identifier->symbol));
                    generate_symbol_push(expression);
                }
                // printf("LF@%s\n", );
            }
            break;
        case AST_NODE_INTEGER:
            //            OUTPUT_CODE_PART("PUSHS ");
            //            generate_integer_push(expression);
            //            OUTPUT_CODE_PART("POPS LF@");
            //            print_symbol(&identifier->symbol);
            //            OUTPUT_CODE_LINE("\n");
            OUTPUT_CODE("MOVE LF@%s ", get_symbol_name(&identifier->symbol));
            generate_integer_push(expression);
            break;
        case AST_NODE_NUMBER:
            OUTPUT_CODE("MOVE LF@%s ", get_symbol_name(&identifier->symbol));
            generate_number_push(expression);
            //            OUTPUT_CODE_PART("PUSHS ");
            //            generate_number_push(expression);
            //            OUTPUT_CODE_PART("POPS LF@");
            //            print_symbol(&identifier->symbol);
            //            OUTPUT_CODE_LINE("\n");
            break;
        case AST_NODE_BOOLEAN:
            OUTPUT_CODE("MOVE LF@%s ", get_symbol_name(&identifier->symbol));
            generate_bool_push(expression);
            //            OUTPUT_CODE_PART("PUSHS ");
            //            generate_bool_push(expression);
            //            OUTPUT_CODE_PART("POPS LF@");
            //            print_symbol(&identifier->symbol);
            //            OUTPUT_CODE_LINE("\n");
            break;
        case AST_NODE_STRING:
            OUTPUT_CODE("MOVE LF@%s ", get_symbol_name(&identifier->symbol));
            generate_string_push(expression);
            //            OUTPUT_CODE_PART("PUSHS ");
            //            generate_string_push(expression);
            //            OUTPUT_CODE_PART("POPS LF@");
            //            print_symbol(&identifier->symbol);
            //            OUTPUT_CODE_LINE("\n");
            break;
        case AST_NODE_NIL:
            OUTPUT_CODE("MOVE LF@%s ", get_symbol_name(&identifier->symbol));
            generate_nil_push();
            //            OUTPUT_CODE_PART("PUSHS ");
            //            generate_nil_push();
            //            OUTPUT_CODE_PART("POPS LF@");
            //            print_symbol(&identifier->symbol);
            //            OUTPUT_CODE_LINE("\n");
            break;
        case AST_NODE_FUNC_CALL:
            stack_push(&stack, identifier);
            if(!expression->next) {
                identifier = identifier->next;
                while(identifier) {
                    stack_push(&stack, identifier);
                    identifier = identifier->next;
                }
            }
            stack_push(&stack, expression);
            // break;
            generate_func_call_assignment_RL(expression, lside_counter - (rside_counter - 1));
            break;
        case AST_NODE_BINOP:
            //            generate_binop_assignment(expression);
            //            OUTPUT_CODE_PART("PUSHS ");
            //            generate_result();
            process_binop_node(expression);
            stack_push(&stack, identifier);
            stack_push(&stack, expression);
            break;
        case AST_NODE_UNOP:
            // generate_unop_assignment(expression);
            // OUTPUT_CODE_PART("PUSHS ");
            // generate_result();
            process_unop_node(expression);

            stack_push(&stack, identifier);
            stack_push(&stack, expression);
            break;
        default:
            break;
        }

        if(!identifier) {
            break;
        }
        expression = expression->next;
        identifier = identifier->next;
    }

    while(!stack_empty(&stack)) {
        ast_node_t *expression = stack_pop(&stack);
        if(expression->node_type == AST_NODE_SYMBOL) {
            OUTPUT_CODE("POPS LF@%s\n", get_symbol_name(&expression->symbol));
            continue;
        }
        ast_node_t *identifier = stack_pop(&stack);

        switch(expression->node_type) {
        case AST_NODE_FUNC_CALL: {
            // int args = lside_counter - (rside_counter - 1);
            // generate_func_call_assignment_RL(expression, lside_counter - (rside_counter -
            // 1));
            OUTPUT_CODE("POPS LF@%s\n", get_symbol_name(&identifier->symbol));
            if(!expression->next) {
                while(!stack_empty(&stack)) {
                    ast_node_t *identifier = stack_pop(&stack);
                    OUTPUT_CODE("POPS LF@%s\n", get_symbol_name(&identifier->symbol));
                }
            }
        } break;
        case AST_NODE_BINOP:
            // generate_binop_assignment(expression);
            // OUTPUT_CODE_PART("PUSHS ");
            // generate_result();
            OUTPUT_CODE_PART("POPS LF@");
            print_symbol(&identifier->symbol);
            OUTPUT_CODE_LINE("\n");
            break;
        case AST_NODE_UNOP:
            // generate_unop_assignment(expression);
            // OUTPUT_CODE_PART("PUSHS ");
            // generate_result();
            OUTPUT_CODE_PART("POPS LF@");
            print_symbol(&identifier->symbol);
            OUTPUT_CODE_LINE("\n");

        default:
            break;
        }
    }

    stack_free(&stack);

    //    int cur_max_exp = rside_counter - 1;
    //    for(int l = 0; l < rside_counter; l++) {
    //        expression = cur_node->assignment.expressions;
    //        for(int k = 0; k < rside_counter; k++) {
    //            if(k == cur_max_exp) {
    //                switch(expression->node_type) {
    //                case AST_NODE_SYMBOL:
    //                    OUTPUT_CODE_PART("PUSHS ");
    //                    generate_symbol_push(expression);
    //                    break;
    //                case AST_NODE_INTEGER:
    //                    OUTPUT_CODE_PART("PUSHS ");
    //                    generate_integer_push(expression);
    //                    break;
    //                case AST_NODE_NUMBER:
    //                    OUTPUT_CODE_PART("PUSHS ");
    //                    generate_number_push(expression);
    //                    break;
    //                case AST_NODE_BOOLEAN:
    //                    OUTPUT_CODE_PART("PUSHS ");
    //                    generate_bool_push(expression);
    //                    break;
    //                case AST_NODE_STRING:
    //                    OUTPUT_CODE_PART("PUSHS ");
    //                    generate_string_push(expression);
    //                    break;
    //                case AST_NODE_NIL:
    //                    OUTPUT_CODE_PART("PUSHS ");
    //                    generate_nil_push();
    //                    break;
    //                case AST_NODE_FUNC_CALL:
    //                    generate_func_call_assignment_RL(expression,
    //                                                     lside_counter - (rside_counter - 1));
    //                    break;
    //                case AST_NODE_BINOP:
    //                    generate_binop_assignment(expression);
    //                    OUTPUT_CODE_PART("PUSHS ");
    //                    generate_result();
    //                    break;
    //                case AST_NODE_UNOP:
    //                    generate_unop_assignment(expression);
    //                    OUTPUT_CODE_PART("PUSHS ");
    //                    generate_result();
    //                    break;
    //                default:
    //                    break;
    //                }
    //            }
    //            expression = expression->next;
    //        }
    //        cur_max_exp--;
    //    }

    //    ast_node_t *identifier = cur_node->assignment.identifiers;
    //    while(identifier) {
    //        OUTPUT_CODE_PART("POPS LF@");
    //        print_symbol(&identifier->symbol);
    //        OUTPUT_CODE_LINE("\n");
    //        identifier = identifier->next;
    //    }
}

void process_node_func_call(ast_node_t *cur_node)
{
    int lside_counter;
    if(strcmp(cur_node->func_call.name.ptr, "write")) { // If it's not write()
        if(cur_node->func_call.def) {
            lside_counter = count_children(cur_node->func_call.def->arguments);
        } else {
            lside_counter = count_children(cur_node->func_call.decl->argument_types);
        }
    } else { // If it's write()
        lside_counter = count_children(cur_node->func_call.arguments);
    }
    int rside_counter = 0;
    ast_node_t *cur_arg = cur_node->func_call.arguments;
    int added_to_write = 0;
    for(int i = 0; i < lside_counter; i++) {
        added_to_write = 1;
        switch(cur_arg->node_type) {
        case AST_NODE_SYMBOL:
            OUTPUT_CODE_PART("PUSHS ");
            generate_symbol_push(cur_arg);
            break;
        case AST_NODE_INTEGER:
            OUTPUT_CODE_PART("PUSHS ");
            generate_integer_push(cur_arg);
            break;
        case AST_NODE_NUMBER:
            OUTPUT_CODE_PART("PUSHS ");
            generate_number_push(cur_arg);
            break;
        case AST_NODE_BOOLEAN:
            OUTPUT_CODE_PART("PUSHS ");
            generate_bool_push(cur_arg);
            break;
        case AST_NODE_STRING:
            OUTPUT_CODE_PART("PUSHS ");
            generate_string_push(cur_arg);
            break;
        case AST_NODE_NIL:
            OUTPUT_CODE_PART("PUSHS ");
            generate_nil_push();
            break;
        case AST_NODE_FUNC_CALL:
            if(cur_arg->func_call.def) {
                added_to_write = count_children(cur_arg->func_call.def->return_types);
            } else {
                added_to_write = count_children(cur_arg->func_call.decl->return_types);
            }
            generate_func_call_assignment(cur_arg, lside_counter - rside_counter);
            break;
        case AST_NODE_BINOP:
            generate_binop_assignment(cur_arg);
            OUTPUT_CODE_PART("PUSHS ");
            generate_result();
            break;
        case AST_NODE_UNOP:
            generate_unop_assignment(cur_arg);
            OUTPUT_CODE_PART("PUSHS ");
            generate_result();
            break;
        default:
            break;
        }
        rside_counter++;
        cur_arg = cur_arg->next;
    }

    for(int j = 0; j < rside_counter - lside_counter; j++) {
        OUTPUT_CODE_LINE("POPS GF@trash"); // Losing unwanted expression results.
    }
    OUTPUT_CODE_LINE("CREATEFRAME");
    if(!strcmp(cur_node->func_call.name.ptr, "write")) {
        lside_counter = lside_counter - 1 + added_to_write;
    }

    for(int l = 0; l < lside_counter; l++) {
        OUTPUT_CODE_LINE("POPS GF@result");
        printf("DEFVAR TF@%%%d\n", lside_counter - 1 - l);
        printf("MOVE TF@%%%d GF@result\n", lside_counter - 1 - l);
    }

    // if not write
    if(strcmp(cur_node->func_call.name.ptr, "write")) {
        OUTPUT_CODE_PART("CALL $");
        printf("%s\n", cur_node->func_call.name.ptr);
    } else {
        generate_write(lside_counter);
    }
    EMPTY_LINE;
}

void eval_condition()
{
    OUTPUT_CODE_LINE("LABEL EVAL_CONDITION");
    OUTPUT_CODE_LINE("POPS GF@result");

    OUTPUT_CODE_LINE("TYPE GF@type1 GF@result");
    OUTPUT_CODE_LINE("JUMPIFEQ IS_FALSE GF@type1 string@nil");
    OUTPUT_CODE_LINE("JUMPIFEQ IS_BOOL GF@type1 string@bool");
    OUTPUT_CODE_LINE("JUMP IS_TRUE"); // All other types are true

    OUTPUT_CODE_LINE("LABEL IS_BOOL");
    OUTPUT_CODE_LINE("JUMPIFEQ IS_FALSE GF@result bool@false"); // bool false == false
    OUTPUT_CODE_LINE("JUMP IS_TRUE");                           // bool true  == true
    OUTPUT_CODE_LINE("JUMP END_EVAL_CHECK");

    OUTPUT_CODE_LINE("LABEL IS_FALSE");
    OUTPUT_CODE_LINE("MOVE GF@result bool@false"); // result = false
    OUTPUT_CODE_LINE("JUMP END_EVAL_CHECK");

    OUTPUT_CODE_LINE("LABEL IS_TRUE");
    OUTPUT_CODE_LINE("MOVE GF@result bool@true"); // result = true
    OUTPUT_CODE_LINE("JUMP END_EVAL_CHECK");

    OUTPUT_CODE_LINE("LABEL END_EVAL_CHECK");
    OUTPUT_CODE_LINE("PUSHS GF@result");
    OUTPUT_CODE_LINE("RETURN");
}

void process_for_node(ast_node_t *for_node)
{
    global_label_counter++;
    int local_label_counter = global_label_counter;
    global_label_counter++;
    int second_local_label_counter = global_label_counter;

    ast_node_t *iterator = for_node->for_loop.iterator;
    ast_node_t *step = for_node->for_loop.step;
    ast_node_t *condition = for_node->for_loop.condition;
    ast_node_t *copy = for_node->for_loop.setup;

    ast_node_t *body = for_node->for_loop.body;

    process_node(iterator, 0);
    process_node(step, 0);
    process_node(condition, 0);
    process_node(copy, 0);

    char *iterator_name = get_symbol_name(&iterator->symbol);
    char *step_name = get_symbol_name(&step->symbol);
    char *condition_name = get_symbol_name(&condition->symbol);
    char *copy_name = get_symbol_name(&copy->symbol);

    // Konvertuj iterator, step, condition na rovnaky typ.
    OUTPUT_CODE_PART("PUSHS ");
    printf("LF@%s\n", iterator_name);
    OUTPUT_CODE_LINE("CALL FOR_CONVERT");
    OUTPUT_CODE_PART("POPS ");
    printf("LF@%s\n", iterator_name);

    OUTPUT_CODE_PART("PUSHS ");
    printf("LF@%s\n", step_name);
    OUTPUT_CODE_LINE("CALL ZERO_STEP");
    OUTPUT_CODE_PART("POPS ");
    printf("LF@%s\n", step_name);

    OUTPUT_CODE_PART("PUSHS ");
    printf("LF@%s\n", condition_name);
    OUTPUT_CODE_LINE("CALL FOR_CONVERT");
    OUTPUT_CODE_PART("POPS ");
    printf("LF@%s\n", condition_name);

    OUTPUT_CODE_PART("LABEL ");
    output_label(local_label_counter);
    OUTPUT_CODE_LINE("");
    OUTPUT_CODE_PART("MOVE ");
    printf("LF@%s ", copy_name);
    printf("LF@%s\n", iterator_name);
    OUTPUT_CODE_PART("MOVE GF@for_condition ");
    printf("LF@%s\n", condition_name);
    OUTPUT_CODE_PART("MOVE GF@for_step ");
    printf("LF@%s\n", step_name);
    OUTPUT_CODE_PART("MOVE GF@for_iter ");
    printf("LF@%s\n", iterator_name);
    OUTPUT_CODE_LINE("CALL SHOULD_I_JUMP");
    OUTPUT_CODE_LINE("POPS GF@result");
    OUTPUT_CODE_PART("JUMPIFEQ ");
    output_label(second_local_label_counter);
    OUTPUT_CODE_LINE(" GF@result bool@true");

    process_node(body, second_local_label_counter);

    OUTPUT_CODE_PART("ADD ");
    printf("LF@%s ", iterator_name);
    printf("LF@%s ", iterator_name);
    printf("LF@%s\n", step_name);
    OUTPUT_CODE_PART("JUMP ");
    output_label(local_label_counter);
    OUTPUT_CODE_LINE("");
    OUTPUT_CODE_PART("LABEL ");
    output_label(second_local_label_counter);
    OUTPUT_CODE_LINE("");
}

/*
if step is negative, jump if iter is lower than condition.
if step is positive, jump if iter is higher than condition.
*/

void should_i_jump()
{
    OUTPUT_CODE_LINE("LABEL SHOULD_I_JUMP");

    OUTPUT_CODE_LINE("LT GF@result GF@for_step float@0x0p+0");
    OUTPUT_CODE_LINE("JUMPIFEQ NEG_STEP GF@result bool@true");
    OUTPUT_CODE_LINE("JUMP POS_STEP");

    OUTPUT_CODE_LINE("LABEL NEG_STEP");
    OUTPUT_CODE_LINE("LT GF@result GF@for_iter GF@for_condition");
    OUTPUT_CODE_LINE("PUSHS GF@result");

    OUTPUT_CODE_LINE("JUMP SHOULD_I_JUMP_END");

    OUTPUT_CODE_LINE("LABEL POS_STEP");
    OUTPUT_CODE_LINE("GT GF@result GF@for_iter GF@for_condition");
    OUTPUT_CODE_LINE("PUSHS GF@result");

    OUTPUT_CODE_LINE("LABEL SHOULD_I_JUMP_END");
    OUTPUT_CODE_LINE("RETURN");
}

void zero_step()
{
    OUTPUT_CODE_LINE("LABEL ZERO_STEP");
    OUTPUT_CODE_LINE("POPS GF@op1");
    OUTPUT_CODE_LINE("TYPE GF@type1 GF@op1");

    OUTPUT_CODE_LINE("JUMPIFEQ stepFIRST_OP_NIL GF@type1 string@nil");
    OUTPUT_CODE_LINE("JUMPIFEQ stepFIRST_OP_INT_conv GF@type1 string@int");
    OUTPUT_CODE_LINE("JUMP stepFLOAT_DONE");
    OUTPUT_CODE_LINE("LABEL stepFIRST_OP_INT_conv");
    OUTPUT_CODE_LINE("INT2FLOAT GF@op1 GF@op1");

    OUTPUT_CODE_LINE("LABEL stepFLOAT_DONE");
    OUTPUT_CODE_LINE("PUSHS GF@op1");
    OUTPUT_CODE_LINE("JUMPIFEQ step_is_zero GF@op1 float@0x0p+0");
    OUTPUT_CODE_LINE("RETURN");

    OUTPUT_CODE_LINE("LABEL step_is_zero");
    OUTPUT_CODE_LINE("EXIT int@6");

    OUTPUT_CODE_LINE("LABEL stepFIRST_OP_NIL");
    OUTPUT_CODE_LINE("EXIT int@7");
}

void for_convert()
{
    OUTPUT_CODE_LINE("LABEL FOR_CONVERT");
    OUTPUT_CODE_LINE("POPS GF@op1");
    OUTPUT_CODE_LINE("TYPE GF@type1 GF@op1");

    OUTPUT_CODE_LINE("JUMPIFEQ forFIRST_OP_NIL GF@type1 string@nil");
    OUTPUT_CODE_LINE("JUMPIFEQ forFIRST_OP_INT_conv GF@type1 string@int");
    OUTPUT_CODE_LINE("JUMP forFLOAT_DONE");
    OUTPUT_CODE_LINE("LABEL forFIRST_OP_INT_conv");
    OUTPUT_CODE_LINE("INT2FLOAT GF@op1 GF@op1");

    OUTPUT_CODE_LINE("LABEL forFLOAT_DONE");
    OUTPUT_CODE_LINE("PUSHS GF@op1");
    OUTPUT_CODE_LINE("RETURN");

    OUTPUT_CODE_LINE("LABEL forFIRST_OP_NIL");
    OUTPUT_CODE_LINE("EXIT int@8");
}

void generate_if_code(ast_node_t *condition, ast_node_t *body, int local_label_counter,
                      int break_label)
{
    global_label_counter++;
    int internal_label = global_label_counter;
    process_node(condition, 0);

    OUTPUT_CODE_LINE("CALL EVAL_CONDITION");

    OUTPUT_CODE_LINE("POPS GF@result");

    OUTPUT_CODE_PART("JUMPIFEQ ");
    output_label(internal_label);
    OUTPUT_CODE_LINE(" GF@result bool@false");

    process_node(body, break_label);

    OUTPUT_CODE_PART("JUMP ");
    output_label(local_label_counter);
    OUTPUT_CODE_LINE("");
    OUTPUT_CODE_PART("LABEL ");
    output_label(internal_label);
    OUTPUT_CODE_LINE("");
}

void process_if_node(ast_node_t *cur_node, int break_label)
{
    global_label_counter++;
    int local_label_counter = global_label_counter;
    ast_node_t *condition = cur_node->if_condition.conditions;
    ast_node_t *body = cur_node->if_condition.bodies;
    while(condition != NULL) {
        generate_if_code(condition, body, local_label_counter, break_label);
        condition = condition->next;
        body = body->next;
    }
    if(body) {
        process_node(body, break_label);
    }
    OUTPUT_CODE_PART("LABEL ");
    output_label(local_label_counter);
    OUTPUT_CODE_LINE("");
}

void process_while_node(ast_node_t *cur_node)
{
    global_label_counter++;
    int local_label_counter = global_label_counter;
    global_label_counter++;
    int second_local_label_counter = global_label_counter;

    ast_node_t *condition = cur_node->while_loop.condition;
    ast_node_t *body = cur_node->while_loop.body;
    OUTPUT_CODE_PART("LABEL ");
    output_label(local_label_counter);
    OUTPUT_CODE_LINE("");
    process_node(condition, 0);
    OUTPUT_CODE_LINE("CALL EVAL_CONDITION");
    OUTPUT_CODE_LINE("POPS GF@result");
    OUTPUT_CODE_PART("JUMPIFEQ ");
    output_label(second_local_label_counter);
    OUTPUT_CODE_LINE(" GF@result bool@false");
    process_node(body, second_local_label_counter);

    OUTPUT_CODE_PART("JUMP ");
    output_label(local_label_counter);
    OUTPUT_CODE_LINE("");
    OUTPUT_CODE_PART("LABEL ");
    output_label(second_local_label_counter);
    OUTPUT_CODE_LINE("");
}

void process_repeat_until(ast_node_t *cur_node)
{
    global_label_counter++;
    int local_label_counter = global_label_counter;
    global_label_counter++;
    int second_local_label_counter = global_label_counter;

    ast_node_t *condition = cur_node->repeat_loop.condition;
    ast_node_t *body = cur_node->repeat_loop.body;
    OUTPUT_CODE_PART("LABEL ");
    output_label(local_label_counter);
    OUTPUT_CODE_LINE("");
    process_node(body, second_local_label_counter);
    process_node(condition, 0);
    OUTPUT_CODE_LINE("CALL EVAL_CONDITION");
    OUTPUT_CODE_LINE("POPS GF@result");
    OUTPUT_CODE_PART("JUMPIFEQ ");
    output_label(local_label_counter);
    OUTPUT_CODE_LINE(" GF@result bool@false");
    OUTPUT_CODE_PART("LABEL ");
    output_label(second_local_label_counter);
    OUTPUT_CODE_LINE("");
}

void process_node(ast_node_t *cur_node, int break_label)
{
    switch(cur_node->node_type) {
    case AST_NODE_INVALID:
        break;
    case AST_NODE_FUNC_DECL:
        // Declarations are ignored in code generator.
        break;
    case AST_NODE_SYMBOL:
        OUTPUT_CODE_PART("PUSHS ");
        ret_id_arg(&(cur_node->symbol));
        break;
    case AST_NODE_INTEGER:
        OUTPUT_CODE_PART("PUSHS ");
        printf("int@%ld\n", cur_node->integer);
        break;
    case AST_NODE_NUMBER:
        OUTPUT_CODE_PART("PUSHS ");
        printf("float@%a\n", cur_node->number);
        break;
    case AST_NODE_STRING:
        OUTPUT_CODE_PART("PUSHS ");
        printf("string@%s\n", cur_node->string.ptr);
        break;
    case AST_NODE_NIL:
        OUTPUT_CODE_LINE("PUSHS nil@nil");
        break;
    case AST_NODE_BOOLEAN:
        OUTPUT_CODE_PART("PUSHS ");
        generate_bool_push(cur_node);
        break;
    case AST_NODE_FUNC_DEF:
        process_node_func_def(cur_node);
        break;

    case AST_NODE_FUNC_CALL:
        process_node_func_call(cur_node);
        break;

    case AST_NODE_DECLARATION:
        process_declaration_node(cur_node, false);
        break;

    case AST_NODE_ASSIGNMENT:
        process_assignment_node(cur_node);
        break;

    case AST_NODE_IF:
        process_if_node(cur_node, break_label);
        break;

    case AST_NODE_WHILE:
        process_while_node(cur_node);
        break;

    case AST_NODE_REPEAT:
        process_repeat_until(cur_node);
        break;

    case AST_NODE_FOR:
        process_for_node(cur_node);
        break;

    case AST_NODE_RETURN:
        process_return_node(cur_node);
        break;

    case AST_NODE_BINOP:
        process_binop_node(cur_node);
        break;
    case AST_NODE_BODY:
        for(ast_node_t *it = cur_node->body.statements; it != NULL; it = it->next) {
            process_node(it, break_label);
        }
        break;
    case AST_NODE_BREAK:
        OUTPUT_CODE_LINE("JUMP "), output_label(break_label), printf("\n");
        break;
    default:
        break;
    }
}

void look_for_declarations(ast_node_t *root)
{

    switch(root->node_type) {
    case AST_NODE_DECLARATION:
        process_declaration_node(root, true);
        break;
    case AST_NODE_BODY: {
        ast_node_t *body_statement = root->body.statements;
        while(body_statement) {
            look_for_declarations(body_statement);
            body_statement = body_statement->next;
        }
        break;
    }
    case AST_NODE_IF: {
        ast_node_list_t bodies = root->if_condition.bodies;
        while(bodies) {
            look_for_declarations(bodies);
            bodies = bodies->next;
        }
    } break;
    case AST_NODE_WHILE:
        look_for_declarations(root->while_loop.body);
        break;
    case AST_NODE_REPEAT:
        look_for_declarations(root->repeat_loop.body);
        break;
    case AST_NODE_FOR:
        look_for_declarations(root->for_loop.iterator);
        look_for_declarations(root->for_loop.condition);
        look_for_declarations(root->for_loop.step);
        look_for_declarations(root->for_loop.setup);
        break;
    default:
        break;
    }
}

void generate_reads()
{
    OUTPUT_CODE_LINE("LABEL $reads");
    OUTPUT_CODE_LINE("PUSHFRAME");
    OUTPUT_CODE_LINE("DEFVAR LF@retval0");
    OUTPUT_CODE_LINE("READ LF@retval0 string");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
}

void generate_readi()
{
    OUTPUT_CODE_LINE("LABEL $readi");
    OUTPUT_CODE_LINE("PUSHFRAME");
    OUTPUT_CODE_LINE("DEFVAR LF@retval0");
    OUTPUT_CODE_LINE("READ LF@retval0 int");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
}

void generate_readn()
{
    OUTPUT_CODE_LINE("LABEL $readn");
    OUTPUT_CODE_LINE("PUSHFRAME");
    OUTPUT_CODE_LINE("DEFVAR LF@retval0");
    OUTPUT_CODE_LINE("READ LF@retval0 float");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
}

void generate_tointeger()
{
    OUTPUT_CODE_LINE("LABEL $tointeger");
    OUTPUT_CODE_LINE("PUSHFRAME");
    OUTPUT_CODE_LINE("DEFVAR LF@retval0");
    OUTPUT_CODE_LINE("DEFVAR LF@param0");

    OUTPUT_CODE_LINE("MOVE LF@param0 LF@%0");

    OUTPUT_CODE_LINE("JUMPIFNEQ TOINT_GOOD LF@param0 nil@nil");
    OUTPUT_CODE_LINE("MOVE LF@retval0 nil@nil");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
    OUTPUT_CODE_LINE("LABEL TOINT_GOOD");
    OUTPUT_CODE_LINE("FLOAT2INT LF@retval0 LF@param0");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
}

void generate_chr()
{
    OUTPUT_CODE_LINE("LABEL $chr");
    OUTPUT_CODE_LINE("PUSHFRAME");
    OUTPUT_CODE_LINE("DEFVAR LF@retval0");
    OUTPUT_CODE_LINE("DEFVAR LF@%param0");
    OUTPUT_CODE_LINE("MOVE LF@%param0 LF@%0");

    OUTPUT_CODE_LINE("JUMPIFEQ CHR_NIL LF@%param0 nil@nil"); // if i is nil

    OUTPUT_CODE_LINE("GT GF@result LF@%param0 int@255");
    OUTPUT_CODE_LINE("JUMPIFEQ CHR_OUT GF@result bool@true");

    OUTPUT_CODE_LINE("LT GF@result LF@%param0 int@0");
    OUTPUT_CODE_LINE("JUMPIFEQ CHR_OUT GF@result bool@true");

    OUTPUT_CODE_LINE("JUMP CHR_OK");
    OUTPUT_CODE_LINE("LABEL CHR_OUT");
    OUTPUT_CODE_LINE("MOVE LF@retval0 nil@nil");
    OUTPUT_CODE_LINE("JUMP CHR_END");
    OUTPUT_CODE_LINE("LABEL CHR_OK");
    OUTPUT_CODE_LINE("INT2CHAR LF@retval0 LF@%param0");
    OUTPUT_CODE_LINE("LABEL CHR_END");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");

    OUTPUT_CODE_LINE("LABEL CHR_NIL");
    OUTPUT_CODE_LINE("EXIT int@8");
}

void generate_ord()
{
    OUTPUT_CODE_LINE("LABEL $ord");
    OUTPUT_CODE_LINE("PUSHFRAME");
    OUTPUT_CODE_LINE("DEFVAR LF@retval0");
    OUTPUT_CODE_LINE("DEFVAR LF@%param0");
    OUTPUT_CODE_LINE("DEFVAR LF@%param1");
    OUTPUT_CODE_LINE("MOVE LF@%param0 LF@%0");
    OUTPUT_CODE_LINE("MOVE LF@%param1 LF@%1");

    OUTPUT_CODE_LINE("JUMPIFEQ ORD_NIL LF@%param0 nil@nil"); // if i is nil
    OUTPUT_CODE_LINE("JUMPIFEQ ORD_NIL LF@%param1 nil@nil"); // if j is nil

    OUTPUT_CODE_LINE("STRLEN GF@trash LF@%param0"); // Get length of string

    OUTPUT_CODE_LINE("GT GF@result LF@%param1 GF@trash"); // If index greater than strlen
    OUTPUT_CODE_LINE("JUMPIFEQ ORD_OUT GF@result bool@true");

    OUTPUT_CODE_LINE("LT GF@result LF@%param1 int@1"); // If index lower than 1
    OUTPUT_CODE_LINE("JUMPIFEQ ORD_OUT GF@result bool@true");

    OUTPUT_CODE_LINE("SUB LF@%param1 LF@%param1 int@1");
    OUTPUT_CODE_LINE("STRI2INT LF@retval0 LF@%param0 LF@%param1");
    OUTPUT_CODE_LINE("JUMP ORD_END");
    OUTPUT_CODE_LINE("LABEL ORD_OUT");
    OUTPUT_CODE_LINE("MOVE LF@retval0 nil@nil");
    OUTPUT_CODE_LINE("LABEL ORD_END");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");

    OUTPUT_CODE_LINE("LABEL ORD_NIL");
    OUTPUT_CODE_LINE("EXIT int@8");
}

void int_zerodivcheck()
{
    OUTPUT_CODE_LINE("LABEL int_zerodivcheck");
    OUTPUT_CODE_LINE("POPS GF@op2");
    OUTPUT_CODE_LINE("JUMPIFEQ $zero_division_int GF@op2 int@0");
    OUTPUT_CODE_LINE("PUSHS GF@op2");
    OUTPUT_CODE_LINE("RETURN");
    OUTPUT_CODE_LINE("LABEL $zero_division_int");
    OUTPUT_CODE_LINE("EXIT int@9");
    OUTPUT_CODE_LINE("RETURN");
}

void float_zerodivcheck()
{
    OUTPUT_CODE_LINE("LABEL float_zerodivcheck");
    OUTPUT_CODE_LINE("POPS GF@op2");
    OUTPUT_CODE_LINE("JUMPIFEQ $zero_division_float GF@op2 float@0x0.0p+0");
    OUTPUT_CODE_LINE("PUSHS GF@op2");
    OUTPUT_CODE_LINE("RETURN");
    OUTPUT_CODE_LINE("LABEL $zero_division_float");
    OUTPUT_CODE_LINE("EXIT int@9");
    OUTPUT_CODE_LINE("RETURN");
}

void nil_check()
{

    OUTPUT_CODE_LINE("LABEL NIL_CHECK");
    OUTPUT_CODE_LINE("POPS GF@op2");
    OUTPUT_CODE_LINE("POPS GF@op1");
    OUTPUT_CODE_LINE("JUMPIFEQ NIL_FOUND GF@op1 nil@nil");
    OUTPUT_CODE_LINE("JUMPIFEQ NIL_FOUND GF@op2 nil@nil");
    OUTPUT_CODE_LINE("PUSHS GF@op1");
    OUTPUT_CODE_LINE("PUSHS GF@op2");
    OUTPUT_CODE_LINE("RETURN");
    OUTPUT_CODE_LINE("LABEL NIL_FOUND");
    OUTPUT_CODE_LINE("EXIT int@8");
}

void check_for_conversion()
{
    OUTPUT_CODE_LINE("LABEL CONV_CHECK");
    OUTPUT_CODE_LINE("POPS GF@op2");
    OUTPUT_CODE_LINE("POPS GF@op1");
    OUTPUT_CODE_LINE("TYPE GF@type1 GF@op1");
    OUTPUT_CODE_LINE("TYPE GF@type2 GF@op2");

    OUTPUT_CODE_LINE("JUMPIFEQ TYPES_OK GF@type1 GF@type2");

    OUTPUT_CODE_LINE("JUMPIFEQ TYPES_OK GF@type1 string@nil");
    OUTPUT_CODE_LINE("JUMPIFEQ TYPES_OK GF@type2 string@nil");

    OUTPUT_CODE_LINE("JUMPIFEQ FIRST_OP_INT GF@type1 string@int");
    OUTPUT_CODE_LINE("JUMPIFEQ SEC_OP_INT GF@type2 string@int");

    OUTPUT_CODE_LINE("LABEL FIRST_OP_INT");
    OUTPUT_CODE_LINE("INT2FLOAT GF@op1 GF@op1");
    OUTPUT_CODE_LINE("JUMP TYPES_OK");

    OUTPUT_CODE_LINE("LABEL SEC_OP_INT");
    OUTPUT_CODE_LINE("INT2FLOAT GF@op2 GF@op2");
    OUTPUT_CODE_LINE("JUMP TYPES_OK");

    OUTPUT_CODE_LINE("LABEL TYPES_OK");
    OUTPUT_CODE_LINE("PUSHS GF@op1");
    OUTPUT_CODE_LINE("PUSHS GF@op2");
    OUTPUT_CODE_LINE("RETURN");
}

void generate_substring()
{
    OUTPUT_CODE_LINE("LABEL $substr");
    OUTPUT_CODE_LINE("PUSHFRAME");
    OUTPUT_CODE_LINE("DEFVAR LF@retval0");
    OUTPUT_CODE_LINE("MOVE LF@retval0 string@");

    OUTPUT_CODE_LINE("DEFVAR LF@%param0");
    OUTPUT_CODE_LINE("DEFVAR LF@%param1");
    OUTPUT_CODE_LINE("DEFVAR LF@%param2");

    OUTPUT_CODE_LINE("DEFVAR LF@iterator");
    OUTPUT_CODE_LINE("DEFVAR LF@stringend");
    OUTPUT_CODE_LINE("DEFVAR LF@letter");

    OUTPUT_CODE_LINE("MOVE LF@%param0 LF@%0");
    OUTPUT_CODE_LINE("MOVE LF@%param1 LF@%1");
    OUTPUT_CODE_LINE("MOVE LF@%param2 LF@%2");

    OUTPUT_CODE_LINE("STRLEN GF@trash LF@%param0"); // Get length of string

    OUTPUT_CODE_LINE("GT GF@result LF@%param1 GF@trash"); // If index i greater than strlen
    OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_OUT GF@result bool@true");

    OUTPUT_CODE_LINE("LT GF@result LF@%param1 int@1"); // If index i lower than 1
    OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_OUT GF@result bool@true");

    OUTPUT_CODE_LINE("GT GF@result LF@%param2 GF@trash"); // If index j greater than strlen
    OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_OUT GF@result bool@true");

    OUTPUT_CODE_LINE("LT GF@result LF@%param2 int@1"); // If index j lower than 1
    OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_OUT GF@result bool@true");

    OUTPUT_CODE_LINE("LT GF@result LF@%param2 LF@%param1"); // If index i bigger than j
    OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_OUT GF@result bool@true");

    OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_NIL LF@%param1 nil@nil"); // if i is nil
    OUTPUT_CODE_LINE("JUMPIFEQ SUBSTR_NIL LF@%param2 nil@nil"); // if j is nil

    OUTPUT_CODE_LINE("MOVE LF@iterator LF@%param1");
    OUTPUT_CODE_LINE("SUB LF@iterator LF@iterator int@1");

    OUTPUT_CODE_LINE("MOVE LF@stringend LF@%param2");
    OUTPUT_CODE_LINE("SUB LF@stringend LF@stringend int@1");

    OUTPUT_CODE_LINE("LABEL LOOP");
    OUTPUT_CODE_LINE("GETCHAR LF@letter LF@%param0 LF@iterator");
    OUTPUT_CODE_LINE("CONCAT LF@retval0 LF@retval0 LF@letter");
    OUTPUT_CODE_LINE("JUMPIFEQ DONE LF@iterator LF@stringend");
    OUTPUT_CODE_LINE("ADD LF@iterator LF@iterator int@1");
    OUTPUT_CODE_LINE("JUMP LOOP");

    OUTPUT_CODE_LINE("LABEL DONE");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");

    OUTPUT_CODE_LINE("LABEL SUBSTR_OUT");
    OUTPUT_CODE_LINE("MOVE LF@retval0 string@");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");

    OUTPUT_CODE_LINE("LABEL SUBSTR_NIL");
    OUTPUT_CODE_LINE("MOVE LF@retval0 nil@nil");
    OUTPUT_CODE_LINE("POPFRAME");
    OUTPUT_CODE_LINE("RETURN");
}

void conv_to_float()
{
    OUTPUT_CODE_LINE("LABEL CONV_TO_FLOAT");
    OUTPUT_CODE_LINE("POPS GF@op2");
    OUTPUT_CODE_LINE("POPS GF@op1");
    OUTPUT_CODE_LINE("TYPE GF@type1 GF@op1");
    OUTPUT_CODE_LINE("TYPE GF@type2 GF@op2");

    OUTPUT_CODE_LINE("JUMPIFEQ FIRST_OP_INT_conv GF@type1 string@int");
    OUTPUT_CODE_LINE("JUMPIFEQ SEC_OP_INT_conv GF@type2 string@int");
    OUTPUT_CODE_LINE("JUMP FLOAT_DONE");
    OUTPUT_CODE_LINE("LABEL FIRST_OP_INT_conv");
    OUTPUT_CODE_LINE("INT2FLOAT GF@op1 GF@op1");
    OUTPUT_CODE_LINE("JUMPIFEQ SEC_OP_INT_conv GF@type2 string@int");
    OUTPUT_CODE_LINE("JUMP FLOAT_DONE");

    OUTPUT_CODE_LINE("LABEL SEC_OP_INT_conv");
    OUTPUT_CODE_LINE("INT2FLOAT GF@op2 GF@op2");
    OUTPUT_CODE_LINE("JUMP FLOAT_DONE");

    OUTPUT_CODE_LINE("LABEL FLOAT_DONE");
    OUTPUT_CODE_LINE("PUSHS GF@op1");
    OUTPUT_CODE_LINE("PUSHS GF@op2");
    OUTPUT_CODE_LINE("RETURN");
}

void check_if_int()
{
    OUTPUT_CODE_LINE("LABEL CHECK_IF_INT");
    OUTPUT_CODE_LINE("POPS GF@op2");
    OUTPUT_CODE_LINE("POPS GF@op1");
    OUTPUT_CODE_LINE("TYPE GF@type1 GF@op1");
    OUTPUT_CODE_LINE("TYPE GF@type2 GF@op2");

    OUTPUT_CODE_LINE("JUMPIFEQ FIRST_OP_INT_OK GF@type1 string@int");
    OUTPUT_CODE_LINE("JUMP WRONG");

    OUTPUT_CODE_LINE("LABEL FIRST_OP_INT_OK");
    OUTPUT_CODE_LINE("JUMPIFEQ SEC_OP_INT_OK GF@type2 string@int");
    OUTPUT_CODE_LINE("JUMP WRONG");

    OUTPUT_CODE_LINE("LABEL SEC_OP_INT_OK");

    OUTPUT_CODE_LINE("PUSHS GF@op1");
    OUTPUT_CODE_LINE("PUSHS GF@op2");

    OUTPUT_CODE_LINE("RETURN");

    OUTPUT_CODE_LINE("LABEL WRONG");
    OUTPUT_CODE_LINE("EXIT int@6");
}
void exponentiation()
{
    OUTPUT_CODE_LINE("LABEL EXPONENTIATION");
    OUTPUT_CODE_LINE("POPS GF@exponent");
    OUTPUT_CODE_LINE("POPS GF@base");
    OUTPUT_CODE_LINE("TYPE GF@type1 GF@base");
    OUTPUT_CODE_LINE("TYPE GF@type2 GF@exponent");

    OUTPUT_CODE_LINE(
        "JUMPIFEQ EXPONENT_INT string@int GF@type2"); // if e is float, we turn it to integer
    OUTPUT_CODE_LINE("FLOAT2INT GF@exponent GF@exponent");
    OUTPUT_CODE_LINE("LABEL EXPONENT_INT");

    OUTPUT_CODE_LINE("JUMPIFEQ FLOAT_BASE string@float GF@type1"); // if base is float, dont convert
                                                                   // to float, otherwise convert it
                                                                   // if it is integer
    OUTPUT_CODE_LINE("INT2FLOAT gf@base gf@base");

    OUTPUT_CODE_LINE("LABEL FLOAT_BASE");
    OUTPUT_CODE_LINE("JUMPIFEQ EXP_ZERO GF@exponent int@0"); // if e == 0 in b^e
    OUTPUT_CODE_LINE("LT GF@stackresult GF@exponent int@0"); // If exponent is smaller than
                                                             // zero, stackresult is true
    OUTPUT_CODE_LINE("JUMPIFEQ POSEXPONENT GF@stackresult bool@false"); // Exponent is positive
    OUTPUT_CODE_LINE(
        "MUL GF@exponent GF@exponent int@-1"); // Negative exponent is turned to positive
    OUTPUT_CODE_LINE("LABEL POSEXPONENT");

    OUTPUT_CODE_LINE("MOVE GF@result GF@base");
    OUTPUT_CODE_LINE("SUB GF@exponent GF@exponent int@1"); // if exponent is 3, i only want to
                                                           // multiply the original value 2 times.
    OUTPUT_CODE_LINE("PUSHS GF@result");

    OUTPUT_CODE_LINE("MOVE GF@loop_iterator int@0");
    OUTPUT_CODE_LINE("LABEL EXP_LOOP_START");
    OUTPUT_CODE_LINE(
        "JUMPIFEQ EXP_LOOP_END GF@loop_iterator GF@exponent"); // for (int i = 0;i<(base i - 1);
    OUTPUT_CODE_LINE("PUSHS GF@base");
    OUTPUT_CODE_LINE("CALL CONV_CHECK");
    OUTPUT_CODE_LINE("MULS");
    OUTPUT_CODE_LINE("ADD GF@loop_iterator GF@loop_iterator int@1"); // i++;)
    OUTPUT_CODE_LINE("JUMP EXP_LOOP_START");

    OUTPUT_CODE_LINE("LABEL EXP_LOOP_END");
    OUTPUT_CODE_LINE("JUMPIFEQ EXIT_EXP_LOOP GF@stackresult bool@false");
    OUTPUT_CODE_LINE("POPS GF@result");
    OUTPUT_CODE_LINE("PUSHS float@0x1p+0");
    OUTPUT_CODE_LINE("PUSHS GF@result");
    OUTPUT_CODE_LINE("DIVS");
    OUTPUT_CODE_LINE("LABEL EXIT_EXP_LOOP");
    OUTPUT_CODE_LINE("RETURN");

    OUTPUT_CODE_LINE("LABEL EXP_ZERO");
    OUTPUT_CODE_LINE("JUMPIFEQ ZERO_ZERO GF@base float@0x0p+0"); // 0^0 is invalid
    OUTPUT_CODE_LINE("MOVE GF@result int@1");                    // a^0 where a != 0 is equal to 1.
    OUTPUT_CODE_LINE("PUSHS GF@result");
    OUTPUT_CODE_LINE("RETURN");

    OUTPUT_CODE_LINE("LABEL ZERO_ZERO");
    OUTPUT_CODE_LINE("EXIT int@6");
}

void conv_to_int()
{
    OUTPUT_CODE_LINE("LABEL CONV_TO_INT");
    OUTPUT_CODE_LINE("POPS GF@op2");
    OUTPUT_CODE_LINE("POPS GF@op1");
    OUTPUT_CODE_LINE("TYPE GF@type1 GF@op1");
    OUTPUT_CODE_LINE("TYPE GF@type2 GF@op2");

    OUTPUT_CODE_LINE("JUMPIFEQ FIRST_OP_FLOAT_conv GF@type1 string@float");
    OUTPUT_CODE_LINE("JUMPIFEQ SEC_OP_FLOAT_conv GF@type2 string@float");
    OUTPUT_CODE_LINE("JUMP INT_DONE");
    OUTPUT_CODE_LINE("LABEL FIRST_OP_FLOAT_conv");
    OUTPUT_CODE_LINE("FLOAT2INT GF@op1 GF@op1");
    OUTPUT_CODE_LINE("JUMPIFEQ SEC_OP_FLOAT_conv GF@type2 string@float");
    OUTPUT_CODE_LINE("JUMP INT_DONE");

    OUTPUT_CODE_LINE("LABEL SEC_OP_FLOAT_conv");
    OUTPUT_CODE_LINE("FLOAT2INT GF@op2 GF@op2");
    OUTPUT_CODE_LINE("JUMP INT_DONE");

    OUTPUT_CODE_LINE("LABEL INT_DONE");
    OUTPUT_CODE_LINE("PUSHS GF@op1");
    OUTPUT_CODE_LINE("PUSHS GF@op2");
    OUTPUT_CODE_LINE("RETURN");
}

void generate_builtin()
{
    // Builtin functions
    if(sem_is_builtin_used("reads") || !optimus_prime) {
        OUTPUT_COMMENT("reads begin\n");
        generate_reads();
        OUTPUT_COMMENT("reads end\n");
        EMPTY_LINE;
    }
    if(sem_is_builtin_used("readi") || !optimus_prime) {
        OUTPUT_COMMENT("readi begin\n");
        generate_readi();
        OUTPUT_COMMENT("readi end\n");
        EMPTY_LINE;
    }
    if(sem_is_builtin_used("readn") || !optimus_prime) {
        OUTPUT_COMMENT("readn begin\n");
        generate_readn();
        OUTPUT_COMMENT("readn end\n");
        EMPTY_LINE;
    }
    if(sem_is_builtin_used("tointeger") || !optimus_prime) {
        OUTPUT_COMMENT("tointeger begin\n");
        generate_tointeger();
        OUTPUT_COMMENT("tointeger end\n");
        EMPTY_LINE;
    }
    if(sem_is_builtin_used("chr") || !optimus_prime) {
        OUTPUT_COMMENT("chr begin\n");
        generate_chr();
        OUTPUT_COMMENT("chr end\n");
        EMPTY_LINE;
    }
    if(sem_is_builtin_used("ord") || !optimus_prime) {
        OUTPUT_COMMENT("ord begin\n");
        generate_ord();
        OUTPUT_COMMENT("ord end\n");
        EMPTY_LINE;
    }
    if(sem_is_builtin_used("substr") || !optimus_prime) {
        OUTPUT_COMMENT("substr begin\n");
        generate_substring();
        OUTPUT_COMMENT("substr end\n");
    }
    // My functions
    EMPTY_LINE;
    OUTPUT_COMMENT("int_zerodivcheck begin\n");
    int_zerodivcheck();
    OUTPUT_COMMENT("int_zerodivcheck end\n");
    EMPTY_LINE;
    OUTPUT_COMMENT("float_zerodivcheck begin\n");
    float_zerodivcheck();
    OUTPUT_COMMENT("float_zerodivcheck end\n");
    EMPTY_LINE;
    OUTPUT_COMMENT("nil_check begin\n");
    nil_check();
    OUTPUT_COMMENT("nil_check end\n");
    EMPTY_LINE;
    OUTPUT_COMMENT("check_for_conversion begin\n");
    check_for_conversion();
    OUTPUT_COMMENT("check_for_conversion end\n");
    EMPTY_LINE;
    OUTPUT_COMMENT("check_nil_write begin\n");
    check_nil_write();
    OUTPUT_COMMENT("check_nil_write end\n");
    EMPTY_LINE;
    OUTPUT_COMMENT("eval_condition begin\n");
    eval_condition();
    OUTPUT_COMMENT("eval_condition end\n");
    EMPTY_LINE;
    OUTPUT_COMMENT("exponentiation begin\n");
    exponentiation();
    OUTPUT_COMMENT("exponentiation end\n");
    EMPTY_LINE;
    OUTPUT_COMMENT("check_if_int begin\n");
    check_if_int();
    OUTPUT_COMMENT("check_if_int end\n");
    EMPTY_LINE;
    OUTPUT_COMMENT("conv_to_float begin\n");
    conv_to_float();
    OUTPUT_COMMENT("conv_to_float end\n");
    EMPTY_LINE;
    OUTPUT_COMMENT("zero_step begin\n");
    zero_step();
    OUTPUT_COMMENT("zero_step end\n");
    EMPTY_LINE;
    OUTPUT_COMMENT("for_convert begin\n");
    for_convert();
    OUTPUT_COMMENT("for_convert end\n");
    EMPTY_LINE;
    OUTPUT_COMMENT("should_i_jump begin\n");
    should_i_jump();
    OUTPUT_COMMENT("should_i_jump end\n");
    EMPTY_LINE;
    OUTPUT_COMMENT("conv_to_int begin\n");
    conv_to_int();
    OUTPUT_COMMENT("conv_to_int end\n");
    EMPTY_LINE;
}

void gen_gf_defvar(int index, char *name)
{
    if(gen_is_used(index)) {
        OUTPUT_CODE("DEFVAR GF@%s\n", name);
    }
}

void generate_header()
{
    OUTPUT_CODE_LINE(".IFJcode21");
    EMPTY_LINE;
    COMMENT("Global variables:");
    OUTPUT_CODE_LINE("DEFVAR GF@result");
    OUTPUT_CODE_LINE("DEFVAR GF@trash");
    gen_gf_defvar(G_GF_STACKRESULT, "stackresult");
    gen_gf_defvar(G_GF_OP1, "op1");
    gen_gf_defvar(G_GF_OP2, "op2");
    gen_gf_defvar(G_GF_TYPE1, "type1");
    gen_gf_defvar(G_GF_TYPE2, "type2");
    gen_gf_defvar(G_GF_STRING0, "string0");
    gen_gf_defvar(G_GF_STRING1, "string1");
    gen_gf_defvar(G_GF_LOOP_ITERATOR, "loop_iterator");
    gen_gf_defvar(G_GF_EXPONENT, "exponent");
    gen_gf_defvar(G_GF_BASE, "base");
    gen_gf_defvar(G_GF_FOR_ITER, "for_iter");
    gen_gf_defvar(G_GF_FOR_CONDITION, "for_condition");
    gen_gf_defvar(G_GF_FOR_STEP, "for_step");

    OUTPUT_CODE_LINE("JUMP $$main");
    EMPTY_LINE;
    COMMENT("Built-in functions:");
}

void process_node_program(ast_node_t *cur_node)
{
    generate_builtin();
    ast_node_t *top_level_definitions = cur_node->program.global_statement_list;
    while(top_level_definitions) {
        if(top_level_definitions->node_type == AST_NODE_FUNC_DEF) {
            process_node(top_level_definitions, 0);
        }
        top_level_definitions = top_level_definitions->next;
    }
    OUTPUT_CODE_LINE("LABEL $$main");
    ast_node_t *top_level_call = cur_node->program.global_statement_list;
    while(top_level_call) {
        if(top_level_call->node_type == AST_NODE_FUNC_CALL) {
            process_node(top_level_call, 0);
        }
        top_level_call = top_level_call->next;
    }
}

void avengers_assembler(ast_node_t *ast)
{
    generate_header();
    process_node_program(ast);
}
