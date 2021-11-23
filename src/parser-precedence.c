#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "parser-precedence-table.h"
#include "dynstring.h"
#include "stack.h"
#include "deque.h"
#include "ast.h"
#include "scanner.h"
#include "parser.h"
#include "parser-generated.h"
#include "semantics.h"

#ifdef DBG

static int dbgseverity = 6;

    #define PRINT(severity, ...)                                                                   \
        if(severity >= dbgseverity) {                                                              \
            fprintf(stderr, __VA_ARGS__);                                                          \
        }

    #define DPRINT(severity, ...)                                                                  \
        if(severity >= dbgseverity) {                                                              \
            fprintf(stderr, "<P%d> ", depth);                                                      \
            fprintf(stderr, __VA_ARGS__);                                                          \
        }

#else

    #define DPRINT(...)                                                                            \
        do {                                                                                       \
        } while(0);

#endif

typedef enum
{
    RULE_UNOP,
    RULE_BINOP,
    RULE_ID,
    RULE_LITERAL,
    RULE_PARENTHESES,
    RULE_FUNC_CALL
} rule_id_t;

typedef enum
{
    FLAG_TERM,
    FLAG_NONTERM
} entry_type;

typedef enum
{
    TERM_BINOP,
    TERM_IDENTIFIER
} term_category;

typedef struct {
    token_t token;
    entry_type type;
    bool mark;
} stack_element_t;

static bool check_rparen(stack_element_t *element)
{
    return element->token.token_type == T_RPAREN;
}

static bool check_lparen(stack_element_t *element)
{
    return element->token.token_type == T_LPAREN;
}

static bool check_nonterm(stack_element_t *element)
{
    return element->type == FLAG_NONTERM;
}

static bool check_binop(stack_element_t *element)
{
    return is_binary_op(element->token.token_type);
}

static bool check_unop(stack_element_t *element)
{
    return is_unary_op(element->token.token_type);
}

static bool check_identifier(stack_element_t *element)
{
    return element->token.token_type == T_IDENTIFIER;
}

static bool check_literal(stack_element_t *element)
{
    switch(element->token.token_type) {
    case T_NIL:
    case T_STRING:
    case T_INTEGER:
    case T_NUMBER:
    case T_BOOL:
        return true;
        break;
    default:
        return false;
        break;
    }
}

typedef bool (*rule_function)(stack_element_t *element);

typedef struct {
    entry_type type;
    union {
        // token_kind term_type;
        term_category term_cat;
    };
} rule_nut_t;

typedef struct {
    // bool push_symbol;
    rule_function condition;
    size_t list_size;
    rule_function list[3];
} rule_t;

typedef struct {
    size_t size;
    int id;
    stack_element_t *array[];
} element_array_t;

static const rule_t rules[] = {
    [RULE_UNOP] = { check_unop, 2, { check_nonterm, check_unop } },
    [RULE_BINOP] = { check_binop, 3, { check_nonterm, check_binop, check_nonterm } }, // binop rule
    [RULE_ID] = { check_identifier, 1, { check_identifier } },                        // id rule
    [RULE_PARENTHESES] = { check_rparen,
                           3,
                           { check_rparen, check_nonterm, check_lparen } }, // rparen rule,
    [RULE_LITERAL] = { check_literal, 1, { check_literal } }
};

static void print_element(stack_element_t *e, stack_element_t *sen, int severity)
{
#ifdef DBG
    if(severity < dbgseverity) {
        return;
    }
    if(!e) {
        return;
    }
    fprintf(stderr, "Element: ");
    if(e->mark) {
        fprintf(stderr, "<");
    }

    if(e == sen) {
        fprintf(stderr, "DOLLAR SENTINEL\n");
    } else {
        switch(e->type) {
        case FLAG_NONTERM:
            fprintf(stderr, "E\n");
            break;
        case FLAG_TERM:
            fprintf(stderr, "%s", term_to_string(e->token.token_type));
            if(e->token.token_type == T_IDENTIFIER) {
                fprintf(stderr, " id: %s", e->token.string.ptr);
            }
            fprintf(stderr, "\n");
            break;
        }
    }
#else
    (void) e;
    (void) sen;
    (void) severity;
#endif
}

static void dbg_print(deque_t *stack, int depth, int severity)
{
#ifdef DBG
    if(severity < dbgseverity) {
        return;
    }
    deque_element_t *e = deque_front_element(stack);

    DPRINT(severity, "Stack: { ");
    while(e) {
        stack_element_t *d = (stack_element_t *) e->data;
        const char *s;
        switch(d->type) {
        case FLAG_TERM:
            s = term_to_string(d->token.token_type);
            break;
        case FLAG_NONTERM:
            s = "E";
            break;
        }
        fprintf(stderr, "  ");
        if(d->mark) {
            fprintf(stderr, "<");
        }
        fprintf(stderr, "%s", s);
        e = e->next;
    }
    fprintf(stderr, " }\n");
#else
    (void) stack;
    (void) depth;
    (void) severity;
#endif
}

stack_element_t *create_element(token_t *token, stack_element_t *sen)
{
    stack_element_t *temp = malloc(sizeof(stack_element_t));
    if(temp) {
        temp->token = *token;
        temp->type = FLAG_TERM;
        temp->mark = false;
        print_element(temp, sen, 1);
    }
    return temp;
}

stack_element_t *create_element_empty(entry_type type)
{
    stack_element_t *temp = malloc(sizeof(stack_element_t));
    if(temp) {
        temp->type = type;
        temp->mark = false;
    }
    return temp;
}

ast_node_binop_type_t term_to_binop_type(term_type_t type)
{
    switch(type) {
    case T_PLUS:
        return AST_NODE_BINOP_ADD;
    case T_MINUS:
        return AST_NODE_BINOP_SUB;
    case T_ASTERISK:
        return AST_NODE_BINOP_MUL;
    case T_SLASH:
        return AST_NODE_BINOP_DIV;
    case T_DOUBLE_SLASH:
        return AST_NODE_BINOP_INTDIV;
    case T_PERCENT:
        return AST_NODE_BINOP_MOD;
    case T_CARET:
        return AST_NODE_BINOP_POWER;
    case T_DOUBLE_DOT:
        return AST_NODE_BINOP_CONCAT;
    case T_AND:
        return AST_NODE_BINOP_AND;
    case T_OR:
        return AST_NODE_BINOP_OR;
    case T_LT:
        return AST_NODE_BINOP_LT;
    case T_LTE:
        return AST_NODE_BINOP_LTE;
    case T_GT:
        return AST_NODE_BINOP_GT;
    case T_GTE:
        return AST_NODE_BINOP_GTE;
    case T_DOUBLE_EQUALS:
        return AST_NODE_BINOP_EQ;
    case T_TILDE_EQUALS:
        return AST_NODE_BINOP_NE;
    default:
        return 0;
    }
}

ast_node_unop_type_t term_to_unop_type(term_type_t type)
{
    switch(type) {
    case T_MINUS:
        return AST_NODE_UNOP_NEG;
    case T_HASH:
        return AST_NODE_UNOP_LEN;
    case T_NOT:
        return AST_NODE_UNOP_NOT;
    default:
        return 0; // TODO
    }
}

bool is_table_terminal(term_type_t type)
{
    for(size_t i = 0; i < sizeof(precedence_terminals) / sizeof(*precedence_terminals); ++i) {
        if(type == precedence_terminals[i]) {
            return true;
        }
    }
    return false;
}

stack_element_t *parser_top(deque_t *stack)
{
    deque_element_t *it = deque_front_element(stack);
    stack_element_t *e = NULL;
    while(it) {
        e = (stack_element_t *) it->data;
        if(e->type == FLAG_TERM) {
            break;
        }
        it = it->next;
    }
    return e;
}

static void free_stack(adt_stack_t *stack)
{
    while(!stack_empty(stack)) {
        element_array_t *a = (element_array_t *) stack_pop(stack);
        for(size_t i = 0; i < a->size; ++i) {
            stack_element_t *e = a->array[i];
            // print_element(e, sen);
            free(e);
        }
        free(a);
    }
    stack_free(stack);
}

static void free_deque(deque_t *stack, stack_element_t *sen)
{
    while(!deque_empty(stack)) {
        stack_element_t *e = (stack_element_t *) deque_pop_front(stack);
        print_element(e, sen, 1);
        free(e);
    }
    deque_free(stack);
}

static void free_parser_bottom_up(deque_t *stack, adt_stack_t *right_analysis, stack_element_t *sen)
{
    free_deque(stack, sen);
    free_stack(right_analysis);
}

static int push_nonterm(deque_t *stack)
{
    stack_element_t *e = create_element_empty(FLAG_NONTERM);
    if(!e) {
        return E_INT;
    }
    return deque_push_front(stack, e);
}

static int execute_rule(int id, deque_t *stack, const rule_t *rule, adt_stack_t *output,
                        stack_element_t *sen, int depth)
{
    (void) depth;

    element_array_t *group =
        calloc(sizeof(element_array_t) + rule->list_size * sizeof(element_array_t *), sizeof(char));
    if(!group) {
        return E_INT;
    }
    group->size = rule->list_size;
    group->id = id;

    if(stack_push(output, group) != E_OK) {
        return E_INT;
    }
    DPRINT(1, "Exec rule: %d\n", id);
    for(size_t i = 0; i < rule->list_size; ++i) {
        DPRINT(1, "Rule i: %lu\n", i);
        dbg_print(stack, 0, 1);
        stack_element_t *e = deque_front(stack);
        if(!e || e->mark) {
            DPRINT(8, "[INTERNAL, PREC_PARSER] Syntax error: sequence underflow.\n");
            return E_SYN;
        }

        if(!rule->list[i](e)) {
            DPRINT(8, "[INTERNAL, PREC_PARSER] Syntax error: wrong nut. \n");
            return E_SYN;
        }

        deque_pop_front(stack);

        group->array[i] = e;
    }

    stack_element_t *e = deque_front(stack);
    print_element(e, sen, 1);
    if(!e->mark) {
        DPRINT(8, "[INTERNAL, PREC_PARSER] Syntax error: sequence overflow.\n");
        return E_SYN;
    }
    e->mark = false;

    if(push_nonterm(stack) != E_OK) {
        return E_INT;
    }

    DPRINT(3, ">>> Pushed rule: %d\n", id);

    return E_OK;
}

static int parser_reduce(deque_t *stack, stack_element_t *top, adt_stack_t *output, int depth,
                         stack_element_t *sen)
{
    (void) depth;

    int r = E_SYN;
    bool executed = false;

    for(size_t i = 0; i < sizeof(rules) / sizeof(rule_t); ++i) {

        if(rules[i].condition(top)) {
            r = execute_rule(i, stack, &rules[i], output, sen, depth);
            executed = true;
            break;
        }
    }
    if(!executed) { // temporary
        DPRINT(8, "[INTERNAL, PREC_PARSER] error: no rule\n");
    }

    // DPRINT("Reduced:\n");
    // dbg_print(stack, depth);
    return r;
}

static int prec_get_next_token(token_t *current, int *level)
{
    int r = get_next_token(current);
    if(r != E_OK) {
        return r;
    }
    if(current->token_type == T_LPAREN) {
        (*level)++;
        // DPRINT("push (: %d\n", (*level));
    }
    if(current->token_type == T_RPAREN) {
        //        DPRINT("pop ): %d\n", (*level));
        (*level)--;
    }
    return E_OK;
}

static int parser_shift(deque_t *stack, token_t *current, int *level, int depth,
                        stack_element_t *sen)
{
    stack_element_t *element = create_element(current, sen);
    // DPRINT("Pushing to P[%d]: ", depth);
    /// print_element(element, sen);
    if(!element || deque_push_front(stack, element) != E_OK) {
        return E_INT;
    }
    dbg_print(stack, depth, 1);
    return prec_get_next_token(current, level);
}

static int dbgcount = 0;

static bool check_condition(deque_t *stack, int depth, stack_element_t *sentinel)
{
    (void) depth;
    stack_element_t *current_top = deque_front(stack);
    DPRINT(1, " >> CHECK: ");
    print_element(current_top, sentinel, 1);
    //    stack_element_t *top = parser_top(stack);
    bool cond = is_binary_op(current_top->token.token_type) ||
                is_unary_op(current_top->token.token_type) ||
                current_top->token.token_type == T_LPAREN;
    // current_top->type == FLAG_NONTERM
    if(!cond && current_top != sentinel) {
        DPRINT(1, "Return contol condition\n");
        return true;
    }
    return false;
}

static int parse_func_call(deque_t *stack, adt_stack_t *output, token_t *current,
                           int *parentheses_level, stack_element_t *sentinel, int depth)
{
    ast_node_t *node = NULL;

    unget_token();
    unget_token();
    int err = parse(NT_FUNC_CALL, &node, 0);
    if(err != E_OK) {
        // free node
        return err;
    }

    prec_get_next_token(current, parentheses_level);
    stack_element_t *top = parser_top(stack);
    (void) top;
    DPRINT(6, ">> Switch to BOTTOM UP (resume)\n");
    DPRINT(3, "  Top: %s\n", term_to_readable(top->token.token_type));
    DPRINT(3, "  Current: %s\n", term_to_readable(current->token_type));
    DPRINT(2, "  PLevel: %d\n", *parentheses_level);

    element_array_t *group =
        calloc(sizeof(element_array_t) + 1 * sizeof(element_array_t *), sizeof(char));
    if(!group) {
        // free node
        return E_INT;
    }
    group->size = 1;
    group->id = RULE_FUNC_CALL;

    if(stack_push(output, group) != E_OK) {
        // free node
        return E_INT;
    }

    group->array[0] = (void *) node;

    stack_element_t *e = deque_front(stack);
    DPRINT(1, "Stopped at: ");
    print_element(e, sentinel, 1);

    if(push_nonterm(stack) != E_OK) {
        return E_INT;
    }

    DPRINT(3, ">>> Pushed rule: %d\n", RULE_FUNC_CALL);

    dbg_print(stack, depth, 1);

    return E_OK;
}

static int parse_loop(deque_t *stack, adt_stack_t *output, int depth, stack_element_t *sentinel)
{
    int parentheses_level = 0;
    int result = E_OK;
    stack_element_t *top = NULL;

    token_t current;
    int r = prec_get_next_token(&current, &parentheses_level);

    if(r != E_OK) {
        DPRINT(7, "Token get error\n");
        return r;
    }
    bool return_control = false;

    while(true) {

        top = parser_top(stack);
        if(!top) {
            DPRINT(9, "[INTERNAL, PREC_PARSER] Invalid stack state? idk should not happen\n");
            return E_INT;
        }

        DPRINT(2, "Top: %s\n", term_to_readable(top->token.token_type));
        DPRINT(2, "Current: %s\n", term_to_readable(current.token_type));
        if(current.token_type == T_IDENTIFIER) {
            DPRINT(2, "  current id: %s\n", current.string.ptr);
        }
        dbg_print(stack, depth, 1);
        if(current.token_type == T_IDENTIFIER && !return_control) {

            return_control = check_condition(stack, depth, sentinel);

            if(!return_control) {
                token_t lookahead;
                get_next_token(&lookahead);
                DPRINT(2, "Lookahead: %s\n", term_to_readable(lookahead.token_type));
                if(lookahead.token_type == T_LPAREN) {
                    DPRINT(6, "<< Switch to TOP DOWN\n");

                    int r = parse_func_call(stack, output, &current, &parentheses_level, sentinel,
                                            depth);
                    if(r != E_OK) {
                        return r;
                    }

                    if(current.token_type == T_IDENTIFIER) {
                        return_control = true;
                    }

                    continue;
                } else {
                    unget_token();
                }
            }
        }

        if(current.token_type == T_RPAREN) {
            if(parentheses_level == -1 && top == sentinel) {
                unget_token();
                DPRINT(6, "<< Switch to TOP DOWN (end of func call)\n");
                break;
            }
        }

        if(current.token_type == T_MINUS) {
            DPRINT(1, ">>>> T_MINUS case: \n");
            stack_element_t *current_top = deque_front(stack);
            print_element(current_top, sentinel, 9);
            dbg_print(stack, depth, 1);
            if(current_top->type != FLAG_NONTERM &&
               (current_top == sentinel || is_binary_op(current_top->token.token_type) ||
                is_unary_op(current_top->token.token_type))) {
                DPRINT(5, ">> SET TO UNARY\n");
                current.token_type = T_MINUS_UNARY;
            }
        }

        token_t token = is_table_terminal(current.token_type) ? current : sentinel->token;

        if(top == sentinel && (token.token_type == T_EOF || return_control)) {
            DPRINT(4, "stopping: end condition\n");
            unget_token();
            break;
        }

        int row = term_to_index(top->token.token_type);
        int col = term_to_index(token.token_type);
        DPRINT(3, "Rule at [%d][%d]\n", row, col);
        int precedence = precedence_table[row][col];

        if(return_control) {
            precedence = PREC_GT;
        }

        switch(precedence) {
        case PREC_EQ:
            DPRINT(3, "case prec_eq\n");
            if(!return_control) {
                result = parser_shift(stack, &current, &parentheses_level, depth, sentinel);
            }
            break;
        case PREC_LT:
            DPRINT(3, "case prec_lt\n");
            if(!return_control) {
                top->mark = true;
                result = parser_shift(stack, &current, &parentheses_level, depth, sentinel);
            }
            break;
        case PREC_GT:
            DPRINT(3, "case prec_gt\n");
            result = parser_reduce(stack, top, output, depth, sentinel);
            break;
        case PREC_ZE:
            DPRINT(3, "case prec_void\n");
            DPRINT(3, "Top: %s\n", term_to_readable(top->token.token_type));
            DPRINT(3, "Current: %s\n", term_to_readable(current.token_type));
            if(current.token_type == T_IDENTIFIER) {
                parser_reduce(stack, top, output, depth, sentinel);
                top = parser_top(stack);
                return_control = true;
                if(top == sentinel) {
                    unget_token();
                    break;
                }
            } else {
                DPRINT(3, " prec_void error\n");
                return E_SEM;
            }
            //            dbg_print(stack, depth);
            break;
        }
        if(result != E_OK) {
            return result;
        }
        dbg_print(stack, depth, 1);

        if(++dbgcount == 1337) { // temp
            DPRINT(9, "[precdence parser safeguard] LOOP LIMIT REACHED\n");
            exit(1);
        }
    }

    if(top != sentinel) {
        dbg_print(stack, depth, 8);
        DPRINT(8, "[INTERNAL, PREC_PARSER] stack not empty:\n");
        dbg_print(stack, depth, 8);
        return E_SEM;
    }

    return result;
}

static int assemble_ast(adt_stack_t *right_analysis, ast_node_t **node, stack_element_t *sen,
                        int depth)
{
    if(stack_empty(right_analysis)) {
        return E_INT;
    }
    *node = calloc(1, sizeof(ast_node_t));

    element_array_t *a = (element_array_t *) stack_pop(right_analysis);
    DPRINT(3, "  Rule %d: {\n", a->id);
    if(a->id == RULE_FUNC_CALL) {
        DPRINT(3, "    %d\n", ((ast_node_t *) a->array[0])->node_type);
    } else {
        for(size_t i = 0; i < a->size; ++i) {
            stack_element_t *e = a->array[i];
            DPRINT(3, "    ");
            print_element(e, sen, 3);
        }
    }
    DPRINT(3, "  }\n");

    switch(a->id) {
    case RULE_UNOP:
        (*node)->node_type = AST_NODE_UNOP;
        (*node)->unop.type = term_to_unop_type(a->array[1]->token.token_type);
        if(assemble_ast(right_analysis, &(*node)->unop.operand, sen, depth) != E_OK) {
            return E_INT;
        }
        break;
    case RULE_BINOP:
        (*node)->node_type = AST_NODE_BINOP;
        (*node)->binop.type = term_to_binop_type(a->array[1]->token.token_type);
        if(assemble_ast(right_analysis, &(*node)->binop.right, sen, depth) != E_OK) {
            return E_INT;
        }
        if(assemble_ast(right_analysis, &(*node)->binop.left, sen, depth) != E_OK) {
            return E_INT;
        }
        break;
    case RULE_ID:
        (*node)->node_type = AST_NODE_SYMBOL;
        (*node)->symbol.name = a->array[0]->token.string;
        (*node)->symbol.is_declaration = true;
        break;
    case RULE_PARENTHESES:
        if(assemble_ast(right_analysis, node, sen, depth)) {
            return E_INT;
        }
        break;
    case RULE_LITERAL: {
        token_t t = a->array[0]->token;
        switch(t.token_type) {
        case T_INTEGER:
            (*node)->node_type = AST_NODE_INTEGER;
            (*node)->integer = t.integer;
            break;
        case T_STRING:
            (*node)->node_type = AST_NODE_STRING;
            (*node)->string = t.string;
            break;
        case T_NUMBER:
            (*node)->node_type = AST_NODE_NUMBER;
            (*node)->number = t.number;
            break;
        case T_BOOL:
            (*node)->node_type = AST_NODE_BOOLEAN;
            (*node)->boolean = t.boolean;
            break;
        case T_NIL:
            (*node)->node_type = AST_NODE_NIL;
            break;
        default:
            return E_INT;
        }
    } break;
    case RULE_FUNC_CALL:
        (*node) = (ast_node_t *) a->array[0];
        break;
    }

    if(a->id != RULE_FUNC_CALL) {
        for(size_t i = 0; i < a->size; ++i) {
            stack_element_t *e = a->array[i];
            free(e);
        }
    }

    free(a);
    return E_OK;
}

static int depth_c = 0;
int precedence_parse(ast_node_t **root)
{

    adt_stack_t right_analysis;
    if(stack_create(&right_analysis, 8) != E_OK) {
        return E_INT;
    }

    deque_t stack;
    deque_create(&stack);
    int depth = depth_c;
    depth_c++;

    stack_element_t *sentinel = malloc(sizeof(stack_element_t));
    sentinel->type = FLAG_TERM;
    sentinel->mark = false;
    sentinel->token.token_type = T_EOF;

    if(deque_push_front(&stack, sentinel) != E_OK) {
        free(sentinel);
        free_parser_bottom_up(&stack, &right_analysis, sentinel);
        return E_INT;
    }

    DPRINT(6, ">> Switch to BOTTOM UP (precedence start)\n");

    int r;
    r = parse_loop(&stack, &right_analysis, depth, sentinel);

    // DPRINT("Precedence parse end. [%d]\n", r);
    if(r == E_OK) {
        DPRINT(5, "Parse successful\n");

        DPRINT(5, "Analysis: {\n");

        assemble_ast(&right_analysis, root, sentinel, depth);

        DPRINT(5, "}\n");

        // r = sem_check_expression(*root);
    } else if(r == E_SYN) {
        DPRINT(5, "Parse syntax error\n");
    } else if(r == E_SEM) {
        DPRINT(5, "Parse exp semantic error\n");
    }

    free_parser_bottom_up(&stack, &right_analysis, sentinel);
    DPRINT(6, "<< Switch to TOP DOWN (precedence end)\n");
    return r;
}
