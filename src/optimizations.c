#include "optimizations.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "error.h"
#include "deque.h"

#ifdef DBG

static int dbgseverity = 1;

    #define PRINT(severity, ...)                                                                   \
        if(severity >= dbgseverity) {                                                              \
            fprintf(stderr, __VA_ARGS__);                                                          \
        }

#else

    #define PRINT(...)                                                                             \
        do {                                                                                       \
        } while(0);
#endif

bool optimus_prime = true;

static bool gen_map[G_GF_FOR_STEP + 1];
static int stage;

typedef struct {
    int counter;
    bool is_cycle;
} scope_t;

static scope_t *scopes = NULL;
static size_t scope_size;
static size_t current_scope;

static void free_scopes()
{
    if(scopes) {
        free(scopes);
        scopes = NULL;
    }
}

static int init_scopes()
{
    free_scopes();
    scopes = calloc(1, sizeof(scope_t));
    if(!scopes) {
        return E_INT;
    }
    scope_size = 1;
    current_scope = 0;
    scopes[current_scope].counter = 0;
    scopes[current_scope].is_cycle = false;
    return E_OK;
}

static void print_scope()
{
#ifdef DBG
    for(size_t i = 0; i <= current_scope; ++i) {
        fprintf(stderr, "[%lu]: ", i);
        if(i == 0) {
            fprintf(stderr, "(global)");
        }
        fprintf(stderr, " loop: %s", scopes[i].is_cycle ? "Y" : "N");
        fprintf(stderr, "\n");
    }
#endif
}

static int push_scope(bool is_cycle)
{
    current_scope++;
    if(current_scope >= scope_size) {
        scope_size *= 2;
        scope_t *temp = (scope_t *) realloc(scopes, scope_size * sizeof(scope_t));
        if(!temp) {
            free(scopes);
            return E_INT;
        }
        scopes = temp;
        scopes[current_scope].counter = 0;
    }
    scopes[current_scope].is_cycle = is_cycle;
    PRINT(3, "opt: pushing scope\n");
    print_scope();
    return E_OK;
}

static void pop_scope()
{
    if(current_scope > 0) {
        current_scope--;
        PRINT(3, "opt: popping scope\n");
        print_scope();
    }
}

static int first_pass(ast_node_t **node);

#include <stdarg.h>
static void gen_add(int amount, ...)
{
    va_list ap;
    va_start(ap, amount);
    for(int i = 0; i < amount; ++i) {
        int index = va_arg(ap, int);
        gen_map[index] = true;
    }
    va_end(ap);
}

static void gen_usage_check_nil_write()
{
    gen_add(2, G_GF_OP1, G_GF_TYPE1);
}

static void gen_usage_for_convert()
{
    gen_add(2, G_GF_OP1, G_GF_TYPE1);
}

static void gen_usage_zero_step()
{
    gen_add(2, G_GF_OP1, G_GF_TYPE1);
}

static void gen_usage_nil_check()
{
    gen_add(2, G_GF_OP1, G_GF_OP2);
}

static void gen_usage_int_zerodivcheck()
{
    gen_add(2, G_GF_OP1, G_GF_OP2);
}

static void gen_usage_float_zerodivcheck()
{
    gen_add(1, G_GF_OP2);
}

static void gen_usage_check_for_conversion()
{
    gen_add(4, G_GF_OP1, G_GF_OP2, G_GF_TYPE1, G_GF_TYPE2);
}

static void gen_usage_conv_to_float()
{
    gen_add(4, G_GF_OP1, G_GF_OP2, G_GF_TYPE1, G_GF_TYPE2);
}

static void gen_usage_conv_to_int()
{
    gen_add(4, G_GF_OP1, G_GF_OP2, G_GF_TYPE1, G_GF_TYPE2);
}

static void gen_usage_check_if_int()
{
    gen_add(4, G_GF_OP1, G_GF_OP2, G_GF_TYPE1, G_GF_TYPE2);
}

static void gen_usage_exponentiation()
{
    gen_add(6, G_GF_EXPONENT, G_GF_BASE, G_GF_TYPE1, G_GF_TYPE2, G_GF_STACKRESULT,
            G_GF_LOOP_ITERATOR);
    gen_usage_check_for_conversion();
}

static void gen_usage_write()
{
    gen_usage_check_nil_write();
}

static void gen_usage_should_i_jump()
{
    gen_add(3, G_GF_FOR_CONDITION, G_GF_FOR_STEP, G_GF_FOR_ITER);
}

static void gen_usage_for()
{
    gen_usage_for_convert();
    gen_usage_zero_step();
    gen_usage_should_i_jump();
    gen_add(3, G_GF_FOR_CONDITION, G_GF_FOR_STEP, G_GF_FOR_ITER);
}

static void gen_usage_unop_node(ast_node_unop_type_t type)
{
    switch(type) {
    case AST_NODE_UNOP_LEN:
        gen_usage_nil_check();
        break;
    case AST_NODE_UNOP_NOT:
        gen_usage_nil_check();
        break;
    case AST_NODE_UNOP_NEG:
        gen_usage_nil_check();
        gen_usage_check_for_conversion();
        break;
    default:
        break;
    }
}

static void gen_usage_binop_node(ast_node_binop_type_t type)
{
    switch(type) {
    case AST_NODE_BINOP_ADD:
        gen_usage_nil_check();
        gen_usage_check_for_conversion();
        break;
    case AST_NODE_BINOP_SUB:
        gen_usage_nil_check();
        gen_usage_check_for_conversion();
        break;
    case AST_NODE_BINOP_MUL:
        gen_usage_nil_check();
        gen_usage_check_for_conversion();
        break;
    case AST_NODE_BINOP_DIV:
        gen_usage_nil_check();
        gen_usage_conv_to_float();
        gen_usage_float_zerodivcheck();
        break;
    case AST_NODE_BINOP_INTDIV:
        gen_usage_nil_check();
        gen_usage_check_if_int();
        gen_usage_int_zerodivcheck();
        break;
    case AST_NODE_BINOP_MOD:
        gen_usage_nil_check();
        gen_usage_conv_to_int();
        gen_usage_int_zerodivcheck();
        break;
    case AST_NODE_BINOP_POWER:
        gen_usage_exponentiation();
        break;
    case AST_NODE_BINOP_LT:
        gen_usage_nil_check();
        gen_usage_check_for_conversion();
        break;
    case AST_NODE_BINOP_GT:
        gen_usage_nil_check();
        gen_usage_check_for_conversion();
        break;
    case AST_NODE_BINOP_LTE:
        gen_usage_nil_check();
        gen_usage_check_for_conversion();
        break;
    case AST_NODE_BINOP_GTE:
        gen_usage_nil_check();
        gen_usage_check_for_conversion();
        break;
    case AST_NODE_BINOP_EQ:
        gen_usage_check_for_conversion();
        break;
    case AST_NODE_BINOP_NE:
        gen_usage_check_for_conversion();
        break;
    case AST_NODE_BINOP_AND:
        break;
    case AST_NODE_BINOP_OR:
        break;
    case AST_NODE_BINOP_CONCAT:
        gen_add(2, G_GF_STRING1, G_GF_STRING0);
        break;
    default:
        break;
    }
}

bool is_not_nil(ast_node_t *node)
{
    if(!node) {
        return false;
    }
    switch(node->node_type) {
    case AST_NODE_SYMBOL:

        if(node->symbol.is_declaration) {
            // should not happen?
            return false;
        } else {
            if(node->symbol.declaration->constant && !node->symbol.declaration->dirty) {
                return is_not_nil(node->symbol.declaration->expr);
            }
            return false;
        }

    case AST_NODE_INTEGER:
    case AST_NODE_NUMBER:
    case AST_NODE_STRING:
    case AST_NODE_BOOLEAN:
        return true;
    case AST_NODE_FUNC_CALL:
        // this is so difficult
        return false;
    default:
        return false;
    }
}

bool is_constant(ast_node_t *node)
{
    if(!node) {
        return false;
    }
    switch(node->node_type) {
    case AST_NODE_SYMBOL:

        if(node->symbol.is_declaration) {
            // should not happen?
            return false;
        } else {
            if(node->symbol.declaration->constant && !node->symbol.declaration->dirty) {
                return true;
            }
            return false;
        }

    case AST_NODE_INTEGER:
    case AST_NODE_NUMBER:
    case AST_NODE_NIL:
    case AST_NODE_STRING:
    case AST_NODE_BOOLEAN:
        return true;
    case AST_NODE_FUNC_CALL:
        // this is so difficult
        return false;
    default:
        return false;
    }
}

int get_literal_int(ast_node_t *node, int64_t *dest)
{

    switch(node->node_type) {
    case AST_NODE_SYMBOL:
        if(node->symbol.is_declaration && node->symbol.constant &&
           node->symbol.type == TYPE_INTEGER && node->symbol.expr) {
            *dest = node->symbol.expr->integer;
            return E_OK;
        }
        return E_INT;
    case AST_NODE_INTEGER:
        *dest = node->integer;
        return E_OK;
    case AST_NODE_NUMBER:
        *dest = (int64_t) node->number;
        return E_OK;
    default:
        return E_INT;
    }
}

int get_literal_number(ast_node_t *node, double *dest)
{

    switch(node->node_type) {
    case AST_NODE_SYMBOL:
        if(node->symbol.is_declaration && node->symbol.constant &&
           node->symbol.type == TYPE_NUMBER && node->symbol.expr) {
            *dest = node->symbol.expr->number;
            return E_OK;
        }
        return E_INT;
    case AST_NODE_INTEGER:
        *dest = (double) node->integer;
        return E_OK;
    case AST_NODE_NUMBER:
        *dest = node->number;
        return E_OK;
    default:
        return E_INT;
    }
}

int get_literal_string(ast_node_t *node, string_t *dest)
{

    switch(node->node_type) {
    case AST_NODE_SYMBOL:
        if(node->symbol.is_declaration && node->symbol.constant &&
           node->symbol.type == TYPE_STRING && node->symbol.expr) {
            *dest = node->symbol.expr->string;
            return E_OK;
        }
        return E_INT;
    case AST_NODE_STRING:
        *dest = node->string;
        return E_OK;
    default:
        return E_INT;
    }
}

int get_literal_bool(ast_node_t *node, bool *dest)
{

    switch(node->node_type) {
    case AST_NODE_SYMBOL:
        if(node->symbol.is_declaration && node->symbol.constant && node->symbol.type == TYPE_BOOL &&
           node->symbol.expr && node->symbol.expr) {
            *dest = node->symbol.expr->boolean;
            return E_OK;
        }
        return E_INT;
    case AST_NODE_BOOLEAN:
        *dest = node->boolean;
        return E_OK;
    default:
        return E_INT;
    }
}

#define E_INT_S (69)

int try_binop_optimalization(ast_node_t *lnode, ast_node_t *rnode, type_t left, type_t right,
                             type_t type, ast_node_t **out)
{
    int r = E_OK;
    ast_node_binop_type_t op = (*out)->binop.type;
    PRINT(4, "Binop opt: result: %s\n", type_to_readable(type));
    switch(type) {
    case TYPE_INTEGER: {
        int64_t lhs;
        r = get_literal_int(lnode, &lhs);
        if(r != E_OK) {
            return E_INT_S;
        }
        int64_t rhs;
        r = get_literal_int(rnode, &rhs);
        if(r != E_OK) {
            return E_INT_S;
        }

        (*out)->node_type = AST_NODE_INTEGER;
        switch(op) {
        case AST_NODE_BINOP_ADD:
            (*out)->integer = lhs + rhs;
            break;
        case AST_NODE_BINOP_SUB:
            (*out)->integer = lhs - rhs;
            break;
        case AST_NODE_BINOP_MUL:
            (*out)->integer = lhs * rhs;
            break;
        case AST_NODE_BINOP_INTDIV:
        case AST_NODE_BINOP_DIV:
            if(rhs == 0) {
                fprintf(stderr, "parser: error: division by 0\n");
                r = E_ZERODIV;
            } else {
                (*out)->integer = lhs / rhs;
            }
            break;
        case AST_NODE_BINOP_MOD:
            if(rhs == 0) {
                fprintf(stderr, "parser: error: division by 0\n");
                r = E_ZERODIV;
            } else {
                (*out)->integer = (int64_t) fmod(lhs, rhs);
            }
            break;
        case AST_NODE_BINOP_POWER:
            (*out)->integer = (int64_t) pow(lhs, rhs);
            break;
        default:
            PRINT(6, "Opt: Warn unhandled binop combination\n");
            return E_INT_S;
        }
    } break;
    case TYPE_NUMBER: {
        double lhs;
        if(left == TYPE_NUMBER) {
            r = get_literal_number(lnode, &lhs);
            if(r != E_OK) {
                return E_INT_S;
            }
        } else if(left == TYPE_INTEGER) {
            int64_t temp;
            r = get_literal_int(lnode, &temp);
            if(r != E_OK) {
                return E_INT_S;
            }
            lhs = (double) temp;
        } else {
            return E_INT_S;
        }

        double rhs;
        if(right == TYPE_NUMBER) {
            r = get_literal_number(rnode, &rhs);
            if(r != E_OK) {
                return E_INT_S;
            }
        } else if(right == TYPE_INTEGER) {
            int64_t temp;
            r = get_literal_int(rnode, &temp);
            if(r != E_OK) {
                return E_INT_S;
            }
            rhs = (double) temp;
        } else {
            return E_INT_S;
        }

        (*out)->node_type = AST_NODE_NUMBER;
        switch(op) {
        case AST_NODE_BINOP_ADD:
            (*out)->number = lhs + rhs;
            break;
        case AST_NODE_BINOP_SUB:
            (*out)->number = lhs - rhs;
            break;
        case AST_NODE_BINOP_MUL:
            (*out)->number = lhs * rhs;
            break;
        case AST_NODE_BINOP_DIV:
            if(rhs == 0) {
                fprintf(stderr, "parser: error: division by 0\n");
                r = E_ZERODIV;
            } else {
                (*out)->number = lhs / rhs;
            }
            break;
        case AST_NODE_BINOP_MOD:
            if(rhs == 0) {
                fprintf(stderr, "parser: error: division by 0\n");
                r = E_ZERODIV;
            } else {
                (*out)->number = fmod(lhs, rhs);
            }
            break;
        case AST_NODE_BINOP_POWER:
            (*out)->number = pow(lhs, rhs);
            break;
        default:
            PRINT(6, "Opt: Warn unhandled binop combination\n");
            return E_INT_S;
        }
    } break;
    case TYPE_STRING: {
        string_t lhs;
        r = get_literal_string(lnode, &lhs);
        if(r != E_OK) {
            return E_INT_S;
        }
        string_t rhs;
        r = get_literal_string(rnode, &rhs);
        if(r != E_OK) {
            return E_INT_S;
        }
        for(char *c = rhs.ptr; c && *c != '\0'; ++c) {
            int r = str_append_char(&lhs, *c);
            if(r != E_OK) {
                str_free(&lhs);
                str_free(&rhs);
                break;
            }
        }

        str_free(&rhs);
        (*out)->node_type = AST_NODE_STRING;
        (*out)->string = lhs;
    } break;
    case TYPE_BOOL: {
        (*out)->node_type = AST_NODE_BOOLEAN;

        if(left == TYPE_BOOL && right == TYPE_BOOL) {
            bool lhs;
            r = get_literal_bool(lnode, &lhs);
            if(r != E_OK) {
                return E_INT_S;
            }
            bool rhs;
            r = get_literal_bool(rnode, &rhs);
            if(r != E_OK) {
                return E_INT_S;
            }
            switch(op) {
            case AST_NODE_BINOP_AND:
                (*out)->boolean = lhs && rhs;
                break;
            case AST_NODE_BINOP_OR:
                (*out)->boolean = lhs || rhs;
                break;
            default:
                PRINT(6, "Opt: Warn unhandled binop combination\n");
                return E_INT_S;
            }
        } else if(left == TYPE_NIL && right == TYPE_NIL) {
            switch(op) {
            case AST_NODE_BINOP_EQ:
                (*out)->boolean = true;
                break;
            case AST_NODE_BINOP_NE:
                (*out)->boolean = false;
                break;
            default:
                PRINT(6, "Opt: Warn unhandled binop combination\n");
                return E_INT_S;
            }
        } else if(is_number_or_integer(left) && is_number_or_integer(right)) {
            double lhs;
            if(left == TYPE_NUMBER) {
                r = get_literal_number(lnode, &lhs);
                if(r != E_OK) {
                    return E_INT_S;
                }
            } else if(left == TYPE_INTEGER) {
                int64_t temp;
                r = get_literal_int(lnode, &temp);
                if(r != E_OK) {
                    return E_INT_S;
                }
                lhs = (double) temp;
            } else {
                return E_INT_S;
            }

            double rhs;
            if(right == TYPE_NUMBER) {
                r = get_literal_number(rnode, &rhs);
                if(r != E_OK) {
                    return E_INT_S;
                }
            } else if(right == TYPE_INTEGER) {
                int64_t temp;
                r = get_literal_int(rnode, &temp);
                if(r != E_OK) {
                    return E_INT_S;
                }
                rhs = (double) temp;
            } else {
                return E_INT_S;
            }

            bool eq = fabs(lhs - rhs) <= __DBL_EPSILON__;

            switch(op) {
            case AST_NODE_BINOP_NE:
                (*out)->boolean = !eq;
                break;
            case AST_NODE_BINOP_EQ:
                (*out)->boolean = eq;
                break;
            case AST_NODE_BINOP_LT:
                (*out)->boolean = (lhs < rhs);
                break;
            case AST_NODE_BINOP_LTE:
                (*out)->boolean = (lhs <= rhs);
                break;
            case AST_NODE_BINOP_GT:
                (*out)->boolean = (lhs > rhs);
                break;
            case AST_NODE_BINOP_GTE:
                (*out)->boolean = (lhs >= rhs);
                break;
            default:
                PRINT(6, "Opt: Warn unhandled binop combination\n");
                return E_INT_S;
            }
        } else if(left == TYPE_STRING && right == TYPE_STRING) {
            string_t lhs;
            r = get_literal_string(lnode, &lhs);
            if(r != E_OK) {
                return E_INT_S;
            }
            string_t rhs;
            r = get_literal_string(rnode, &rhs);
            if(r != E_OK) {
                return E_INT_S;
            }
            switch(op) {
            case AST_NODE_BINOP_NE:
                (*out)->boolean = strcmp(lhs.ptr, rhs.ptr) == 0;
                break;
            case AST_NODE_BINOP_EQ:
                (*out)->boolean = strcmp(lhs.ptr, rhs.ptr) == 0;
                break;
            case AST_NODE_BINOP_LT:
                (*out)->boolean = strcmp(lhs.ptr, rhs.ptr) < 0;
                break;
            case AST_NODE_BINOP_LTE:
                (*out)->boolean = strcmp(lhs.ptr, rhs.ptr) <= 0;
                break;
            case AST_NODE_BINOP_GT:
                (*out)->boolean = strcmp(lhs.ptr, rhs.ptr) > 0;
                break;
            case AST_NODE_BINOP_GTE:
                (*out)->boolean = strcmp(lhs.ptr, rhs.ptr) >= 0;
                break;
            default:
                PRINT(6, "Opt: Warn unhandled binop combination\n");
                return E_INT_S;
            }
        } else {
            PRINT(6, "Opt: Warn unhandled binop combination\n");
            return E_INT_S;
        }

    } break;
    case TYPE_NIL: {
        // todo
        PRINT(6, "Opt: Warn unhandled binop combination\n");
        return E_INT_S;
    } break;
    }
    return r;
}

static int temp_check_expression(ast_node_t **node, type_t *type, bool is_cond);

int try_unop_optimalization(ast_node_t *operand, type_t optype, type_t *type, ast_node_t **out)
{
    (void) type;
    int r = E_OK;
    ast_node_unop_type_t op = (*out)->unop.type;
    PRINT(4, "Unop opt: result: %s\n", type_to_readable(*type));
    switch(op) {
    case AST_NODE_UNOP_LEN: {
        switch(optype) {
        case TYPE_STRING: {
            string_t str;
            r = get_literal_string(operand, &str);
            if(r != E_OK) {
                return E_INT_S;
            }

            int64_t len = str.length;
            str_free(&str);
            (*out)->node_type = AST_NODE_INTEGER;
            (*out)->integer = len;
        } break;
        default:
            PRINT(6, "Opt: Warn unhandled unopcombination\n");
            return E_INT_S;
            break;
        }

    } break;
    case AST_NODE_UNOP_NEG:
        switch(optype) {
        case TYPE_INTEGER: {
            int64_t v;
            r = get_literal_int(operand, &v);
            if(r != E_OK) {
                return E_INT_S;
            }
            (*out)->node_type = AST_NODE_INTEGER;
            (*out)->integer = -v;
        } break;
        case TYPE_NUMBER: {
            {
                double v;
                r = get_literal_number(operand, &v);
                if(r != E_OK) {
                    return E_INT_S;
                }
                (*out)->node_type = AST_NODE_NUMBER;
                (*out)->number = -v;
            }
            break;
        default:
            PRINT(6, "Opt: Warn unhandled unopcombination\n");
            return E_INT_S;
            break;
        } break;
        }
        break;
    case AST_NODE_UNOP_NOT: {
        switch(optype) {
        case TYPE_BOOL: {
            bool v;
            r = get_literal_bool(operand, &v);
            if(r != E_OK) {
                return E_INT_S;
            }
            (*out)->node_type = AST_NODE_BOOLEAN;
            (*out)->boolean = !v;
            break;
        }
        default:
            PRINT(6, "Opt: Warn unhandled unopcombination\n");
            return E_INT_S;
        }
        break;
    }
    }
    return r;
}

int opt_unop_node(ast_node_t **node, type_t *type, bool is_cond)
{
    int r = temp_check_expression(&(*node)->unop.operand, type, is_cond);
    if(r != E_OK) {
        return r;
    }
    if(is_constant((*node)->unop.operand)) {

        PRINT(3, "Unop can be optimized\n");
        ast_node_t *operand = (*node)->unop.operand;
        type_t optype = *type;

        ast_unop_t old = (*node)->unop;

        *type = old.metadata.result;

        int try = try_unop_optimalization(operand, optype, type, node);
        if(try == E_OK) {
            free(operand); // ast_free todo
        } else {
            // failed to optimalize, but we can continue
            PRINT(4, "Unop node: graceful reset\n");
            (*node)->node_type = AST_NODE_UNOP;
            (*node)->unop = old;
            r = try;
            if(try == E_INT_S) {
                r = E_OK;
            }
            gen_usage_unop_node((*node)->unop.type);
        }

    } else {
        gen_usage_unop_node((*node)->unop.type);
    }
    return r;
}

int opt_binop_node(ast_node_t **node, type_t *type, bool is_cond)
{
    int r;

    type_t left;
    r = temp_check_expression(&(*node)->binop.left, &left, is_cond);
    if(r != E_OK) {
        return r;
    }
    type_t right;
    r = temp_check_expression(&(*node)->binop.right, &right, is_cond);
    if(r != E_OK) {
        return r;
    }

    r = E_OK;
    bool leftc = is_constant((*node)->binop.left);
    bool rightc = is_constant((*node)->binop.right);
    if(leftc && rightc) {
        PRINT(3, "Binop can be optimized\n");

        ast_node_t *lnode = (*node)->binop.left;
        ast_node_t *rnode = (*node)->binop.right;

        ast_binop_t old = (*node)->binop;

        *type = old.metadata.result;

        int try = try_binop_optimalization(lnode, rnode, left, right, *type, node);

        if(try == E_OK) {
            free(lnode); // ast_free todo
            free(rnode); // ast_free todo
        } else {
            // failed to optimalize, but we can continue
            PRINT(4, "Binop node: graceful reset\n");
            (*node)->node_type = AST_NODE_BINOP;
            (*node)->binop = old;
            r = try;
            if(try == E_INT_S) {
                r = E_OK;
            }
            gen_usage_binop_node((*node)->binop.type);
        }
    } else {
        gen_usage_binop_node((*node)->binop.type);
    }

    return r;
}

static int opt_func_call(ast_node_t **node, type_t *type, bool is_cond)
{

    ast_node_t *args = (*node)->func_call.arguments;
    while(args) {
        type_t type;
        int r = temp_check_expression(&args, &type, is_cond);
        if(r != E_OK) {
            return r;
        }
        args = args->next;
    }
    *type = sem_get_func_call_type(*node);
    return E_OK;
}

static int opt_symbol(ast_node_t **node, type_t *type, bool is_cond)
{
    PRINT(3, "  on symbol: %s\n", (*node)->symbol.declaration->name.ptr);
    (void) is_cond;
    bool dec = true;
    //    if(scopes[current_scope].is_cycle) {
    //        dec = false;
    //    }
    //    for(size_t i = 0; i <= current_scope; ++i) {
    //        if(scopes[i].is_cycle) {
    //            dec = false;
    //            break;
    //        }
    //    }

    *type = (*node)->symbol.declaration->type;

    if(!(*node)->symbol.dirty && is_constant((*node)) && dec) {
        PRINT(3, "    CP: can be propagated\n");

        ast_node_t *expr = (*node)->symbol.declaration->expr;
        if(expr) {
            // should be freed

            ast_node_t *next = (*node)->next;
            switch(expr->node_type) {
            case AST_NODE_STRING: {
                string_t cpy;
                if(str_create(expr->string.ptr, &cpy) == E_OK) {
                    (*node)->node_type = AST_NODE_STRING;
                    (*node)->string = cpy;
                }
            } break;
            case AST_NODE_INTEGER:
                (*node)->node_type = AST_NODE_INTEGER;
                (*node)->integer = expr->integer;
                break;
            case AST_NODE_NUMBER:
                (*node)->node_type = AST_NODE_NUMBER;
                (*node)->number = expr->number;
                break;
            case AST_NODE_BOOLEAN:
                (*node)->node_type = AST_NODE_BOOLEAN;
                (*node)->boolean = expr->boolean;
                break;
            case AST_NODE_NIL:
                (*node)->node_type = AST_NODE_NIL;
                break;
            default:
                PRINT(3, "CP: propagation graceful fail\n");
                break;
            }
            (*node)->next = next;
        }
    }

    return E_OK;
}

static int temp_check_expression(ast_node_t **node, type_t *type, bool is_cond)
{
    PRINT(3, "OPT: on expression\n");
    if(!*node) {
        PRINT(3, "  null node\n");
        return E_OK;
    }

    PRINT(3, "  Node: %s\n", node_type_to_readable((*node)->node_type));

    int r = E_OK;
    switch((*node)->node_type) {
    case AST_NODE_BINOP:
        r = opt_binop_node(node, type, is_cond);
        break;
    case AST_NODE_UNOP:
        r = opt_unop_node(node, type, is_cond);
        break;
    case AST_NODE_SYMBOL:
        r = opt_symbol(node, type, is_cond);
        break;
    case AST_NODE_FUNC_CALL:
        r = opt_func_call(node, type, is_cond);
        break;
    case AST_NODE_INTEGER:
        *type = TYPE_INTEGER;
        break;
    case AST_NODE_NUMBER:
        *type = TYPE_NUMBER;
        break;
    case AST_NODE_NIL:
        *type = TYPE_NIL;
        break;
    case AST_NODE_STRING:
        *type = TYPE_STRING;
        break;
    case AST_NODE_BOOLEAN:
        *type = TYPE_BOOL;
        break;
    default:
        break;
    }
    return r;
}

typedef int (*ast_callback)(ast_node_t **node);

// static int first_pass(ast_node_t **node);

static int first_pass_condition(ast_node_t **node)
{
    type_t type;
    int r = temp_check_expression(node, &type, true);
    return r;
}

static int first_pass_expression(ast_node_t **node)
{
    type_t type;
    int r = temp_check_expression(node, &type, false);
    return r;
}

static int iterate_list(ast_node_list_t list, ast_callback callback)
{
    while(list) {
        int r = callback(&list);
        if(r != E_OK) {
            return r;
        }
        list = list->next;
    }
    return E_OK;
}

static void free_node_content(ast_node_t *node)
{
    if(!node) {
        return;
    }
    switch(node->node_type) {
    case AST_NODE_PROGRAM:
        str_free(&node->program.require);
        free_ast(node->program.global_statement_list);
        break;
    case AST_NODE_FUNC_DECL:
        str_free(&node->func_decl.name);
        free_ast(node->func_decl.argument_types);
        free_ast(node->func_decl.return_types);
        break;
    case AST_NODE_FUNC_DEF:
        str_free(&node->func_def.name);
        free_ast(node->func_def.arguments);
        free_ast(node->func_def.return_types);
        free_ast(node->func_def.body);
        break;
    case AST_NODE_FUNC_CALL:
        str_free(&node->func_call.name);
        free_ast(node->func_call.arguments);
        break;
    case AST_NODE_DECLARATION:
        if(node->symbol.is_declaration) {
            str_free(&node->symbol.name);
        }
        free_ast(node->declaration.assignment);
        break;
    case AST_NODE_ASSIGNMENT:
        free_ast(node->assignment.identifiers);
        free_ast(node->assignment.expressions);
        break;
    case AST_NODE_BODY:
        free_ast(node->body.statements);
        break;
    case AST_NODE_IF:
        free_ast(node->if_condition.conditions);
        free_ast(node->if_condition.bodies);
        break;
    case AST_NODE_WHILE:
        free_ast(node->while_loop.condition);
        free_ast(node->while_loop.body);
        break;
    case AST_NODE_REPEAT:
        free_ast(node->repeat_loop.condition);
        free_ast(node->repeat_loop.body);
        break;
    case AST_NODE_FOR:
        free_ast(node->for_loop.iterator);
        free_ast(node->for_loop.setup);
        free_ast(node->for_loop.condition);
        free_ast(node->for_loop.step);
        free_ast(node->for_loop.body);
        break;
    case AST_NODE_RETURN:
        free_ast(node->return_values.values);
        break;
    case AST_NODE_SYMBOL:
        if(node->symbol.is_declaration) {
            str_free(&node->symbol.name);
        }
        break;
    case AST_NODE_STRING:
        str_free(&node->string);
        break;
    case AST_NODE_BINOP:
        free_ast(node->binop.left);
        free_ast(node->binop.right);
        break;
    case AST_NODE_UNOP:
        free_ast(node->unop.operand);
        break;
    default:
        break;
    }
}

static void invalidate_node(ast_node_t *node)
{
    if(!node) {
        return;
    }
    ast_node_t *next = node->next;
    free_node_content(node);
    node->node_type = AST_NODE_INVALID;
    node->next = next;
}

static int opt_func_def(ast_node_t **node, ast_callback callback)
{
    if(!is_function_used(&(*node)->func_def)) {
        PRINT(3, "DEC: Func def: %s\n", (*node)->func_def.name.ptr);
        invalidate_node(*node);

        return E_OK;
    }
    push_scope(false);
    PRINT(3, "func def start\n");
    int r = callback(&(*node)->func_def.body);
    PRINT(3, "func def end\n");
    pop_scope();
    return r;
}

static int opt_declaration(ast_node_t **node)
{
    if(!(*node)->declaration.symbol.used) {
        PRINT(3, "DEC: Declaration: %s\n", (*node)->declaration.symbol.name.ptr);
        invalidate_node(*node);
        return E_OK;
    }

    if((*node)->declaration.symbol.current_read == 0) {
        PRINT(3, "DEC: value of %s not used\n", (*node)->declaration.symbol.name.ptr);
        invalidate_node((*node)->declaration.assignment);
        (*node)->declaration.assignment = NULL;
    }

    ast_node_t *exp = (*node)->declaration.assignment;
    int r = first_pass_expression(&exp);
    if(r != E_OK) {
        return r;
    }
    if(is_constant(exp)) {
        PRINT(3, "decl: has constant assignment\n");
        (*node)->declaration.symbol.constant = true;
        (*node)->declaration.symbol.expr = exp;
    }
    return r;
}

static bool is_condition_const_true(ast_node_t *node)
{
    if(node->node_type == AST_NODE_BOOLEAN && node->boolean == true) {
        return true;
    } else {
        return false;
    }
}

static bool is_condition_const_false(ast_node_t *node)
{
    if(node->node_type == AST_NODE_BOOLEAN && node->boolean == false) {
        return true;
    } else {
        return false;
    }
}

static int opt_if(ast_node_t **node)
{

    PRINT(3, "on if\n");
    ast_node_t *cond = (*node)->if_condition.conditions;
    ast_node_t *body = (*node)->if_condition.bodies;
    ast_node_t *prev_cond = NULL;
    ast_node_t *prev_body = NULL;
    while(cond && body) {

        PRINT(3, "  on condition\n");
        type_t type;
        int r = temp_check_expression(&cond, &type, true);
        if(r != E_OK) {
            return r;
        }

        if(is_condition_const_false(cond)) {
            PRINT(3, " DEC: if branch is always false\n");
            if(!prev_cond) {
                (*node)->if_condition.conditions = cond->next;
            }
            if(!prev_body) {
                (*node)->if_condition.bodies = body->next;
            } else {
                prev_cond->next = cond->next;
                prev_body->next = body->next;
            }

            ast_node_t *tempcond = cond;
            ast_node_t *tempbody = body;

            cond = cond->next;
            body = body->next;

            free_node_content(tempcond);
            free(tempcond);
            free_node_content(tempbody);
            free(tempbody);
        } else if(is_condition_const_true(cond)) {
            PRINT(3, " DEC: if branch is always true\n");
            if(!prev_cond) {
                (*node)->if_condition.conditions = NULL;
            } else {
                prev_cond->next = NULL;
            }
            free_ast(cond);
            free_ast(body->next);
            body->next = NULL;
            push_scope(false);
            r = first_pass(&body);
            pop_scope();
            if(r != E_OK) {
                return r;
            }
        } else {
            push_scope(false);
            r = first_pass(&body);
            pop_scope();
            if(r != E_OK) {
                return r;
            }
            prev_cond = cond;
            prev_body = body;
            cond = cond->next;
            body = body->next;
        }
    }

    if(!(*node)->if_condition.conditions) {
        PRINT(3, "  if: all branches were invalidated!\n");
        if((*node)->if_condition.bodies) {
            PRINT(3, "  else branch remaining!\n");
            (*node)->node_type = AST_NODE_BODY;
            (*node)->body.statements = (*node)->if_condition.bodies;
        }
    }

    return E_OK;
}

static int opt_while(ast_node_t **node)
{
    PRINT(3, "on while\n");

    ast_node_t *cond = (*node)->while_loop.condition;
    type_t type;
    int r = temp_check_expression(&cond, &type, true);
    if(r != E_OK) {
        return r;
    }

    if(is_condition_const_false(cond)) {
        PRINT(3, "  loop is always false\n");
        invalidate_node(*node);
    } else if(is_condition_const_true(cond)) {
        PRINT(5, "Warning: loop condition is always true.\n");
    }

    push_scope(true);
    r = first_pass(&(*node)->while_loop.body);
    pop_scope();
    return r;
}

static int opt_for(ast_node_t **node)
{
    int r = E_OK;
    r = first_pass_condition(&(*node)->for_loop.condition->declaration.assignment);
    if(r != E_OK) {
        return r;
    }
    r = first_pass_expression(&(*node)->for_loop.iterator->declaration.assignment);
    if(r != E_OK) {
        return r;
    }
    r = first_pass_expression(&(*node)->for_loop.setup->declaration.assignment);
    if(r != E_OK) {
        return r;
    }
    r = first_pass_expression(&(*node)->for_loop.step->declaration.assignment);
    if(r != E_OK) {
        return r;
    }
    gen_usage_for();

    push_scope(true);
    r = first_pass(&(*node)->for_loop.body);
    pop_scope();
    return r;
}

static int opt_repeat(ast_node_t **node)
{
    int r = E_OK;
    first_pass_condition(&(*node)->repeat_loop.condition);
    if(r != E_OK) {
        return r;
    }

    push_scope(true);
    r = first_pass(&(*node)->repeat_loop.body);
    pop_scope();
    return r;
}

static int opt_assignment(ast_node_t **node)
{
    PRINT(3, "on assignment.\n");

    bool dec = true;
    if(scopes[current_scope].is_cycle) {
        dec = false;
    }

    bool all_unused = true;
    ast_node_t *ids = (*node)->assignment.identifiers;
    while(ids) {
        if(ids->symbol.current_read != 0) {
            all_unused = false;
            break;
        }
        ids = ids->next;
    }

    if(dec && all_unused) {
        PRINT(3, "  DEC: whole assignment.\n");
        invalidate_node(*node);
        return E_OK;
    }

    ast_node_t *exp = (*node)->assignment.expressions;
    while(exp && exp->next) {
        exp = exp->next;
    }
    if(exp && exp->node_type == AST_NODE_FUNC_CALL) {
        PRINT(3, "  not optimizing: has func call.\n");
        dec = false;
    }

    if(dec) {
        ast_node_t *ids = (*node)->assignment.identifiers;
        ast_node_t *exp = (*node)->assignment.expressions;
        ast_node_t *prev_ids = NULL;
        ast_node_t *prev_exp = NULL;
        while(ids && exp) {
            if(ids->symbol.current_read == 0) {
                PRINT(3, "  var %s is not used.\n", ids->symbol.declaration->name.ptr);

                if(!prev_ids) {
                    (*node)->assignment.identifiers = ids->next;
                } else {
                    prev_ids->next = ids->next;
                }
                if(!prev_exp) {
                    (*node)->assignment.expressions = exp->next;
                } else {
                    prev_exp->next = exp->next;
                }
                ast_node_t *temp_ids = ids;
                ast_node_t *temp_exp = exp;
                ids = ids->next;
                exp = exp->next;
                free_ast(temp_ids);
                free_ast(temp_exp);
            } else {
                prev_ids = ids;
                prev_exp = exp;
                ids = ids->next;
                exp = exp->next;
            }
        }
    }

    return iterate_list((*node)->assignment.expressions, first_pass_expression);
}

static int fp_opt_func_call(ast_node_t **node)
{
    type_t type;
    return opt_func_call(node, &type, false);
}

static int first_pass(ast_node_t **node)
{
    int r = E_OK;
    if(!*node) {
        return r;
    }
    switch((*node)->node_type) {
    case AST_NODE_PROGRAM:
        return iterate_list((*node)->program.global_statement_list, first_pass);
    case AST_NODE_FUNC_DEF:
        return opt_func_def(node, first_pass);
    case AST_NODE_BODY:
        return iterate_list((*node)->body.statements, first_pass);
    case AST_NODE_ASSIGNMENT:
        return opt_assignment(node);
    case AST_NODE_DECLARATION:
        return opt_declaration(node);
    case AST_NODE_IF:
        return opt_if(node);
    case AST_NODE_WHILE:
        return opt_while(node);
    case AST_NODE_FOR:
        return opt_for(node);
    case AST_NODE_FUNC_CALL:
        return fp_opt_func_call(node);
    case AST_NODE_REPEAT:
        return opt_repeat(node);
    default:
        break;
    }
    return r;
}

bool is_function_used(ast_func_def_t *def)
{
    bool used = def->used;
    if(def->decl) {
        used |= def->decl->used;
    }
    return used;
}

int optimize_ast(ast_node_t *node)
{
    if(init_scopes() != E_OK) {
        return E_INT;
    }
    stage = 1;
    for(size_t i = 0; i < sizeof(gen_map) / sizeof(*gen_map); ++i) {
        gen_map[i] = false;
    }
    int r = first_pass(&node);

    if(sem_is_builtin_used("write")) {
        gen_usage_write();
    }

    free_scopes();
    return r;
}

bool gen_is_used(int index)
{
    if(!optimus_prime) {
        return true;
    }
    return gen_map[index];
}
