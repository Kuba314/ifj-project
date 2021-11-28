#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "parser-generated.h"

#include "scanner.h"
#include "ast.h"
#include "error.h"

static ast_node_t **node_list_append(ast_node_list_t *node_list, ast_node_t *node)
{
    while(*node_list) {
        node_list = &(*node_list)->next;
    }
    *node_list = node;
    return node_list;
}
static ast_node_t **node_list_tail(ast_node_list_t *node_list)
{
    while(*node_list) {
        node_list = &(*node_list)->next;
    }
    return node_list;
}
static ast_node_t *alloc_id_node(string_t id)
{
    ast_node_t *node = calloc(1, sizeof(ast_node_t));
    if(node == NULL) {
        return NULL;
    }
    node->node_type = AST_NODE_IDENTIFIER;
    node->identifier = id;
    return node;
}
static ast_node_t *alloc_type_node(type_t type)
{
    ast_node_t *node = calloc(1, sizeof(ast_node_t));
    if(node == NULL) {
        return NULL;
    }
    node->node_type = AST_NODE_TYPE;
    node->type = type;
    return node;
}
static ast_node_t *alloc_type_id_node(string_t id)
{
    ast_node_t *node = calloc(1, sizeof(ast_node_t));
    if(node == NULL) {
        return NULL;
    }
    node->node_type = AST_NODE_ID_TYPE_PAIR;
    node->id_type_pair.id = id;
    return node;
}
static ast_node_t *alloc_break_node()
{
    ast_node_t *node = calloc(1, sizeof(ast_node_t));
    if(node == NULL) {
        return NULL;
    }
    node->node_type = AST_NODE_BREAK;
    return node;
}

#include <stdarg.h>
static void print(int depth, const char *str, ...)
{
    va_list args;
    va_start(args, str);
    for(int i = 0; i < depth << 2; i++) {
        fputc(' ', stderr);
    }
    vfprintf(stderr, str, args);
    fputc('\n', stderr);
    va_end(args);
}

static int put_term(ast_node_t **root, token_t token, nterm_type_t parent_nterm, int depth)
{
    bool error = false;
    static ast_node_t **last_root = NULL;

    switch(token.token_type) {
    case T_STRING:
        switch(parent_nterm) {
        case NT_PROGRAM:
            (*root)->program.require = token.string;
            break;
        default:
            error = true;
        }
        break;
    case T_IDENTIFIER:
        switch(parent_nterm) {
        case NT_FUNC_CALL:
            (*root)->func_call.name = alloc_id_node(token.string);
            break;
        case NT_FUNC_DECL:
            (*root)->func_decl.name = alloc_id_node(token.string);
            break;
        case NT_FUNC_DEF:
            (*root)->func_def.name = alloc_id_node(token.string);
            break;
        case NT_FOR_LOOP:
            (*root)->for_loop.iterator = alloc_id_node(token.string);
            break;
        case NT_IDENTIFIER_WITH_TYPE:
            last_root = node_list_append(root, alloc_type_id_node(token.string));
            break;
        case NT_IDENTIFIER_LIST:
        case NT_IDENTIFIER_LIST2:
            node_list_append(root, alloc_id_node(token.string));
            break;
        case NT_STATEMENT:
            // ignore, will be handled once we know if we're in assignment or func-call
            break;
        default:
            error = true;
        }
        break;
    case T_TYPE:
        switch(parent_nterm) {
        case NT_TYPE_LIST:
        case NT_TYPE_LIST2:
            node_list_append(root, alloc_type_node(token.type));
            break;
        case NT_FUNC_TYPE_LIST:
        case NT_FUNC_TYPE_LIST2:
            node_list_append(root, alloc_type_node(token.type));
            break;
        case NT_IDENTIFIER_WITH_TYPE:
            (*last_root)->id_type_pair.type = token.type;
            break;
        default:
            error = true;
        }
        break;
    case T_ELSE:
        // have to increase if_condition's count in order to save statement-list to bodies and not
        // to conditions
        (*root)->visited_children++;
        break;
    case T_BREAK:
        node_list_append(root, alloc_break_node());
    default:
        break;
    }
    if(error) {
        print(depth, "error: no AST rule for \"%s\" and token type %s",
              nterm_to_readable(parent_nterm), term_to_readable(token.token_type));
        return E_SYN;
    }
    return E_OK;
}
static ast_node_type_t nterm_to_ast_type(nterm_type_t nterm_type)
{
    switch(nterm_type) {
    case NT_PROGRAM:
        return AST_NODE_PROGRAM;
    case NT_DECLARATION:
        return AST_NODE_DECLARATION;
    case NT_ASSIGNMENT:
        return AST_NODE_ASSIGNMENT;
    case NT_FUNC_DECL:
        return AST_NODE_FUNC_DECL;
    case NT_FUNC_DEF:
        return AST_NODE_FUNC_DEF;
    case NT_FUNC_CALL:
        return AST_NODE_FUNC_CALL;
    case NT_COND_STATEMENT:
        return AST_NODE_IF;
    case NT_WHILE_LOOP:
        return AST_NODE_WHILE;
    case NT_FOR_LOOP:
        return AST_NODE_FOR;
    case NT_REPEAT_UNTIL:
        return AST_NODE_REPEAT;
    case NT_RETURN_STATEMENT:
        return AST_NODE_RETURN;
    case NT_STATEMENT_LIST:
        return AST_NODE_BODY;
    default:
        return AST_NODE_INVALID;
    }
}
static int alloc_nterm(nterm_type_t nterm, ast_node_t **root, int depth)
{
    ast_node_type_t node_type = nterm_to_ast_type(nterm);

    // don't allocate nterm if it doesn't have an AST counterpart
    if(node_type == AST_NODE_INVALID) {
        return E_OK;
    }

    // allocate ast node
    print(depth, "alloc %s", nterm_to_readable(nterm));
    *root = calloc(1, sizeof(ast_node_t));
    if(*root == NULL) {
        return E_INT;
    }
    (*root)->node_type = node_type;
    return E_OK;
}

static ast_node_t **get_node_ref(ast_node_t **root, nterm_type_t nterm, int depth)
{
    print(depth, "& %s", nterm_to_readable(nterm));
    switch(nterm) {
    case NT_PROGRAM:
        return &(*root)->program.global_statement_list;
    case NT_DECLARATION:
        switch((*root)->visited_children++) {
        case 0:
            return &(*root)->declaration.id_type_pair;
        case 1:
            return &(*root)->declaration.assignment;
        }
        break;
    case NT_ASSIGNMENT:
        switch((*root)->visited_children++) {
        case 0:
            return &(*root)->assignment.identifiers;
        case 1:
            return &(*root)->assignment.expressions;
        }
        break;
    case NT_FUNC_DECL:
        switch((*root)->visited_children++) {
        case 0:
            return &(*root)->func_decl.argument_types;
        case 1:
            return &(*root)->func_decl.return_types;
        }
        break;
    case NT_FUNC_DEF:
        switch((*root)->visited_children++) {
        case 0:
            return &(*root)->func_def.arguments;
        case 1:
            return &(*root)->func_def.return_types;
        case 2:
            return &(*root)->func_def.body;
        }
        break;
    case NT_FUNC_CALL:
        return &(*root)->func_call.arguments;
    case NT_COND_STATEMENT:
    case NT_COND_OPT_ELSEIF:;
        switch((*root)->visited_children++ % 3) {
        case 0:
            return node_list_tail(&(*root)->if_condition.conditions);
        case 1:
            return node_list_tail(&(*root)->if_condition.bodies);
        case 2:
            return root;
        }
        break;
    case NT_WHILE_LOOP:
        switch((*root)->visited_children++) {
        case 0:
            return &(*root)->while_loop.condition;
        case 1:
            return &(*root)->while_loop.body;
        }
        break;
    case NT_REPEAT_UNTIL:
        switch((*root)->visited_children++) {
        case 0:
            return &(*root)->repeat_loop.body;
        case 1:
            return &(*root)->repeat_loop.condition;
        }
        break;
    case NT_FOR_LOOP:
        switch((*root)->visited_children++) {
        case 0:
            return &(*root)->for_loop.setup;
        case 1:
            return &(*root)->for_loop.condition;
        case 2:
            return &(*root)->for_loop.step;
        case 3:
            return &(*root)->for_loop.body;
        }
        break;
    case NT_RETURN_STATEMENT:
        return &(*root)->return_values.values;
    case NT_STATEMENT_LIST:
        return node_list_tail(&(*root)->body.statements);
    // case NT_IDENTIFIER_WITH_TYPE:
    // case NT_DECL_OPTIONAL_ASSIGNMENT:
    // case NT_IDENTIFIER_LIST_WITH_TYPES:
    // case NT_TYPE_LIST:
    // case NT_FUNC_TYPE_LIST:
    // case NT_RET_EXPRESSION_LIST:

    // case NT_IDENTIFIER_LIST_WITH_TYPES2:
    // case NT_TYPE_LIST2:
    // case NT_FUNC_TYPE_LIST2:
    // case NT_FUN_EXPRESSION_LIST2:
    case NT_GLOBAL_STATEMENT_LIST:
    case NT_OPTIONAL_FUN_EXPRESSION_LIST:
    case NT_STATEMENT_LIST2:
    case NT_FUN_EXPRESSION_LIST2:
    case NT_IDENTIFIER_LIST:
    case NT_IDENTIFIER_LIST2:
    case NT_EXPRESSION_LIST:
    case NT_EXPRESSION_LIST2:
    case NT_RET_EXPRESSION_LIST:
    case NT_RET_EXPRESSION_LIST2:
        return node_list_tail(root);

    // explicit fall throughs
    case NT_OPTIONAL_FOR_STEP: // stay at for node
    case NT_STATEMENT:         // wait for certain keyword to distinguish asts
    case NT_GLOBAL_STATEMENT:  // wait for certain keyword to distinguish asts
        return root;
    default:
        printf("warn: fall through %s\n", nterm_to_readable(nterm));
        return root;
    }
    fprintf(stderr, "error: parser-int: unknown nterm_ref of %s\n", nterm_to_readable(nterm));
    return NULL;
}

int parse(nterm_type_t nterm, ast_node_t **root, int depth)
{
    print(depth, "expanding %s", nterm_to_readable(nterm));

    // call precedence parser if expression encountered
    if(nterm == NT_EXPRESSION) {
        return precedence_parse(root);
    }

    // peek at token
    token_t token;
    if(get_next_token(&token)) {
        fprintf(stderr, "error: parser: couldn't read next token while expanding %s\n",
                nterm_to_readable(nterm));
        return E_SYN;
    }

    // handle func-call special case
    if(nterm == NT_PAREN_EXP_LIST_OR_ID_LIST2) {
        unget_token();
        unget_token();
        if(token.token_type == T_LPAREN) {
            return parse(NT_FUNC_CALL, root, depth);
        } else if(token.token_type == T_COMMA || token.token_type == T_EQUALS) {
            return parse(NT_ASSIGNMENT, root, depth);
        }
    }

    // get expansion list from nterm and following token
    exp_list_t exp_list = table->data[parser_get_table_index(nterm, token.token_type)];
    if(!exp_list.valid) {
        // invalid rule
        fprintf(stderr, "error: parser: invalid rule for %s and %s\n", nterm_to_readable(nterm),
                term_to_readable(token.token_type));
        return E_SYN;
    }

    print(depth, "(%s, %s)", nterm_to_readable(nterm), term_to_readable(token.token_type));

    // put token back
    unget_token();

    alloc_nterm(nterm, root, depth);

    // loop over expansions
    for(size_t i = 0; i < exp_list.size; i++) {

        nut_type_t expected = exp_list.data[i];

        // call recursively if nterm
        if(expected.is_nterm) {
            ast_node_t **ref = get_node_ref(root, nterm, depth);
            if(ref == NULL) {
                fprintf(stderr, "error: parser-int: %s returned a null ref\n",
                        nterm_to_readable(nterm));
                return E_INT;
            }
            int err = parse(expected.nterm, ref, depth + 1);
            if(err) {
                return err;
            }
        } else {
            get_next_token(&token);

            // this parser expects `nil` only as a type
            if(token.token_type == T_NIL) {
                token.token_type = T_TYPE;
                token.type = TYPE_NIL;
            }

            // try to following token with the expected one
            if(token.token_type != expected.term) {
                fprintf(stderr, "error: parser: expected \"%s\", but got \"%s\"\n",
                        term_to_readable(expected.term), term_to_readable(token.token_type));
                return E_SYN;
            } else {
                if(put_term(root, token, nterm, depth)) {
                    return E_SYN;
                }
            }
        }
    }
    return E_OK;
}

static void print_ast_list(int depth, ast_node_list_t list)
{
    while(list) {
        print_ast(depth, list);
        list = list->next;
    }
}
void print_ast(int depth, ast_node_t *root)
{
    switch(root->node_type) {
    case AST_NODE_FUNC_DECL:
        print(depth, "func-decl:");
        print(depth + 1, "name:");
        print_ast(depth + 2, root->func_decl.name);
        print(depth + 1, "return_types:");
        print_ast_list(depth + 2, root->func_decl.return_types);
        print(depth + 1, "argument_types:");
        print_ast_list(depth + 2, root->func_decl.argument_types);
        break;
    case AST_NODE_FUNC_DEF:
        print(depth, "func-def:");
        print(depth + 1, "name:");
        print_ast(depth + 2, root->func_def.name);
        print(depth + 1, "return_types:");
        print_ast_list(depth + 2, root->func_def.return_types);
        print(depth + 1, "arguments:");
        print_ast_list(depth + 2, root->func_def.arguments);
        print_ast(depth + 2, root->func_def.body);
        break;
    case AST_NODE_FUNC_CALL:
        print(depth, "func-call:");
        print(depth + 1, "name:");
        print_ast(depth + 2, root->func_call.name);
        print(depth + 1, "arguments:");
        print_ast_list(depth + 2, root->func_call.arguments);
        break;
    case AST_NODE_DECLARATION:
        print(depth, "decl:");
        print_ast(depth + 1, root->declaration.id_type_pair);
        if(root->declaration.assignment) {
            print(depth + 1, "assign:");
            print_ast(depth + 2, root->declaration.assignment);
        }
        break;
    case AST_NODE_ASSIGNMENT:
        print(depth, "assignment:");
        print(depth + 1, "identifiers:");
        print_ast_list(depth + 2, root->assignment.identifiers);
        print(depth + 1, "expressions:");
        print_ast_list(depth + 2, root->assignment.expressions);
        break;
    case AST_NODE_PROGRAM:
        print(depth, "prog:");
        print(depth + 1, "require: %s", root->program.require.ptr);
        print(depth + 1, "global_statements:");
        print_ast_list(depth + 2, root->program.global_statement_list);
        break;
    case AST_NODE_BODY:
        print(depth, "body:");
        print_ast_list(depth + 1, root->body.statements);
        break;
    case AST_NODE_ID_TYPE_PAIR:
        print(depth, "%s: %s", root->id_type_pair.id.ptr,
              type_to_readable(root->id_type_pair.type));
        break;
    case AST_NODE_IF:
        print(depth, "if:");
        ast_node_list_t conds = root->if_condition.conditions;
        ast_node_list_t bodies = root->if_condition.bodies;
        while(conds != NULL && bodies != NULL) {
            print(depth + 1, "cond:");
            print_ast(depth + 2, conds);
            print_ast(depth + 1, bodies);
            conds = conds->next;
            bodies = bodies->next;
        }
        if(bodies) {
            print_ast(depth + 1, bodies);
        }
        break;
    case AST_NODE_WHILE:
        print(depth, "while:");
        print(depth + 1, "cond:");
        print_ast(depth + 2, root->while_loop.condition);
        print_ast(depth + 1, root->while_loop.body);
        break;
    case AST_NODE_REPEAT:
        print(depth, "repeat:");
        print(depth + 1, "cond:");
        print_ast(depth + 2, root->repeat_loop.condition);
        print_ast(depth + 1, root->repeat_loop.body);
        break;
    case AST_NODE_FOR:
        print(depth, "for:");
        print(depth + 1, "iterator:");
        print_ast(depth + 2., root->for_loop.iterator);
        print(depth + 1, "setup:");
        print_ast(depth + 2, root->for_loop.setup);
        print(depth + 1, "condition:");
        print_ast(depth + 2, root->for_loop.condition);
        if(root->for_loop.step) {
            print(depth + 1, "step:");
            print_ast(depth + 2, root->for_loop.condition);
        }
        print_ast(depth + 1, root->for_loop.body);
        break;
    case AST_NODE_BREAK:
        print(depth, "break");
        break;
    case AST_NODE_RETURN:
        print(depth, "return:");
        ast_node_t *values = root->return_values.values;
        while(values) {
            print_ast(depth + 1, values);
            values = values->next;
        }
        break;
    case AST_NODE_BINOP:
        print(depth, "binop: %d", root->binop.type);
        print_ast(depth + 1, root->binop.left);
        print_ast(depth + 1, root->binop.right);
        break;
    case AST_NODE_UNOP:
        print(depth, "unop: %d", root->unop.type);
        print_ast(depth + 1, root->unop.operand);
        break;
    case AST_NODE_TYPE:
        print(depth, "type: %s", type_to_readable(root->type));
        break;
    case AST_NODE_INTEGER:
        print(depth, "int: %d", root->integer);
        break;
    case AST_NODE_NUMBER:
        print(depth, "number: %.02f", root->number);
        break;
    case AST_NODE_BOOLEAN:
        print(depth, "bool: %s", root->boolean ? "true" : "false");
        break;
    case AST_NODE_IDENTIFIER:
        print(depth, "id: %s", root->identifier.ptr);
        break;
    case AST_NODE_STRING:
        print(depth, "str: \"%s\"", root->string.ptr);
        break;
    case AST_NODE_NIL:
        print(depth, "nil");
        break;
    case AST_NODE_INVALID:
        print(depth, "INVALID NODE");
        break;
    }
}
void free_ast(ast_node_t *root)
{
    if(root == NULL) {
        return;
    }
    switch(root->node_type) {
    case AST_NODE_PROGRAM:
        str_free(&root->program.require);
        free_ast(root->program.global_statement_list);
        break;
    case AST_NODE_FUNC_DECL:
        free_ast(root->func_decl.name);
        free_ast(root->func_decl.argument_types);
        free_ast(root->func_decl.return_types);
        break;
    case AST_NODE_FUNC_DEF:
        free_ast(root->func_def.name);
        free_ast(root->func_def.arguments);
        free_ast(root->func_def.return_types);
        free_ast(root->func_def.body);
        break;
    case AST_NODE_FUNC_CALL:
        free_ast(root->func_call.name);
        free_ast(root->func_call.arguments);
        break;
    case AST_NODE_DECLARATION:
        free_ast(root->declaration.id_type_pair);
        free_ast(root->declaration.assignment);
        break;
    case AST_NODE_ASSIGNMENT:
        free_ast(root->assignment.identifiers);
        free_ast(root->assignment.expressions);
        break;
    case AST_NODE_BODY:
        free_ast(root->body.statements);
        break;
    case AST_NODE_IF:
        free_ast(root->if_condition.conditions);
        free_ast(root->if_condition.bodies);
        break;
    case AST_NODE_WHILE:
        free_ast(root->while_loop.condition);
        free_ast(root->while_loop.body);
        break;
    case AST_NODE_REPEAT:
        free_ast(root->repeat_loop.condition);
        free_ast(root->repeat_loop.body);
        break;
    case AST_NODE_FOR:
        free_ast(root->for_loop.iterator);
        free_ast(root->for_loop.setup);
        free_ast(root->for_loop.condition);
        free_ast(root->for_loop.step);
        free_ast(root->for_loop.body);
        break;
    case AST_NODE_RETURN:
        free_ast(root->return_values.values);
        break;
    case AST_NODE_IDENTIFIER:
        str_free(&root->identifier);
        break;
    case AST_NODE_STRING:
        str_free(&root->string);
        break;
    case AST_NODE_BINOP:
        free_ast(root->binop.left);
        free_ast(root->binop.right);
        break;
    case AST_NODE_UNOP:
        free_ast(root->unop.operand);
        break;
    case AST_NODE_ID_TYPE_PAIR:
        str_free(&root->id_type_pair.id);
        break;
    case AST_NODE_BREAK:
    case AST_NODE_TYPE:
    case AST_NODE_INTEGER:
    case AST_NODE_NUMBER:
    case AST_NODE_BOOLEAN:
    case AST_NODE_NIL:
    case AST_NODE_INVALID:
        break;
    }

    free_ast(root->next);
    free(root);
}
