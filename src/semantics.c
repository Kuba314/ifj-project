#include "semantics.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// todo: labels, forward declarations

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

#ifdef DBG
static const char *node_type_to_readable(ast_node_type_t type)
{
    switch(type) {
    case AST_NODE_INVALID:
        return "AST_NODE_INVALID";

    case AST_NODE_FUNC_DECL:
        return "AST_NODE_FUNC_DECL";
    case AST_NODE_FUNC_DEF:
        return "AST_NODE_FUNC_DEF";
    case AST_NODE_FUNC_CALL:
        return "AST_NODE_FUNC_CALL";
    case AST_NODE_DECLARATION:
        return "AST_NODE_DECLARATION";
    case AST_NODE_ASSIGNMENT:
        return "AST_NODE_ASSIGNMENT";

    case AST_NODE_PROGRAM:
        return "AST_NODE_PROGRAM";

    case AST_NODE_BODY:
        return "AST_NODE_BODY";
    case AST_NODE_SYMBOL:
        return "AST_NODE_SYMBOL";

    case AST_NODE_IF:
        return "AST_NODE_IF";
    case AST_NODE_WHILE:
        return "AST_NODE_WHILE";
    case AST_NODE_FOR:
        return "AST_NODE_FOR";
    case AST_NODE_REPEAT:
        return "AST_NODE_REPEAT";

    case AST_NODE_BREAK:
        return "AST_NODE_BREAK";
    case AST_NODE_RETURN:
        return "AST_NODE_RETURN";

    case AST_NODE_BINOP:
        return "AST_NODE_BINOP";
    case AST_NODE_UNOP:
        return "AST_NODE_UNOP";
    case AST_NODE_TYPE:
        return "AST_NODE_TYPE";

    case AST_NODE_INTEGER:
        return "AST_NODE_INTEGER";
    case AST_NODE_NUMBER:
        return "AST_NODE_NUMBER";
    case AST_NODE_BOOLEAN:
        return "AST_NODE_BOOLEAN";
    case AST_NODE_STRING:
        return "AST_NODE_STRING";
    case AST_NODE_NIL:
        return "AST_NODE_NIL";
    default:
        return "N/A";
    }
}
#endif

static ast_func_def_t *current_def;

static int check_expression(ast_node_t **node, type_t *type);

static int sem_get_type(ast_node_t *node, type_t *dest)
{

    switch(node->node_type) {
    case AST_NODE_TYPE:
        *dest = node->type;
        break;
    case AST_NODE_SYMBOL:
        if(node->symbol.is_declaration) {
            *dest = node->symbol.type;
        } else {
            *dest = node->symbol.declaration->type;
        }
        break;
    case AST_NODE_INTEGER:
        *dest = TYPE_INTEGER;
        break;
    case AST_NODE_NUMBER:
        *dest = TYPE_NUMBER;
        break;
    case AST_NODE_NIL:
        *dest = TYPE_NIL;
        break;
    case AST_NODE_BOOLEAN:
        *dest = TYPE_BOOL;
        break;
    case AST_NODE_STRING:
        *dest = TYPE_STRING;
        break;
    case AST_NODE_BINOP:
        *dest = node->binop.metadata.result;
        break;
    case AST_NODE_UNOP:
        *dest = node->unop.metadata.result;
        break;
    default:
        return E_INT;
        break;
    }
    return E_OK;
}

static bool is_number_or_integer(type_t type)
{
    return type == TYPE_NUMBER || type == TYPE_INTEGER;
}

int get_binop_type(type_t left, type_t right, type_t *result)
{

    if(left == TYPE_NIL || right == TYPE_NIL) {
        return E_NIL;
    } else if(left == TYPE_INTEGER && right == TYPE_INTEGER) {
        *result = TYPE_INTEGER;
    } else if(is_number_or_integer(left) && is_number_or_integer(right)) {
        *result = TYPE_NUMBER;
    } else if(left != right) {
        return E_TYPE_EXPR;
    } else {
        *result = left;
    }
    return E_OK;
}

int sem_get_func_call_type(ast_node_t *node, type_t *type)
{
    if(!node->func_call.def->return_types) {
        return E_TYPE_CALL;
    }
    *type = node->func_call.def->return_types->type;
    return E_OK;
}

ast_func_decl_t *get_func_decl_ref(ast_node_t *node)
{
    if(node->node_type == AST_NODE_FUNC_DEF) {
        return node->func_def.decl;
    } else if(node->node_type == AST_NODE_FUNC_DECL) {
        return &node->func_decl;
    }
    return NULL;
}

ast_func_def_t *get_func_def_ref(ast_node_t *node)
{
    if(node->node_type == AST_NODE_FUNC_DECL) {
        return node->func_decl.def;
    } else if(node->node_type == AST_NODE_FUNC_DEF) {
        return &node->func_def;
    }
    return NULL;
}

ast_node_list_t get_func_return_types(ast_node_t *node)
{
    if(node->node_type == AST_NODE_FUNC_DECL) {
        return node->func_decl.return_types;
    } else if(node->node_type == AST_NODE_FUNC_DEF) {
        return node->func_def.return_types;
    }
    return NULL;
}

int check_variable(ast_node_t *node, bool read, bool write)
{
    if(node->node_type != AST_NODE_SYMBOL) {
        PRINT(3, "check_variable: got wrong type! (%s)\n", node_type_to_readable(node->node_type));
        return E_INT;
    }

    if(node->symbol.is_declaration) {
        char *name = node->symbol.name.ptr;

        PRINT(3, "checking variable: %s\n", name);
        ast_node_t *sym = symtable_find(name);
        if(!sym) {
            PRINT(3, "SEM Error: variable %s not defined\n", name);
            return E_UNDEF;
        }

        symbol_t *declaration = NULL;
        if(sym->node_type == AST_NODE_SYMBOL) {
            declaration = &sym->symbol;
        } else if(sym->node_type == AST_NODE_DECLARATION) {
            declaration = &sym->declaration.symbol;
        } else {
            return E_INT;
        }

        PRINT(3, "Mapped to: %s , %s\n", declaration->name.ptr,
              type_to_readable(declaration->type));
        str_free(&node->symbol.name);
        node->symbol.declaration = declaration;
        node->symbol.is_declaration = false;

        //        if(read) {
        //            declaration->used = true;
        //        }
        //        if(write) {
        //            declaration->dirty = true;
        //        }
        (void) read;
        (void) write;

    } else {
        PRINT(3, "[SEM]                  !!!!! Duplicate check: Identifier: %s\n",
              node->symbol.declaration->name.ptr);
    }
    return E_OK;
}
/*
int get_expression_type(ast_node_t *node, type_t *type)
{
    printf("WIP, dont use yet!\n");
    return E_INT;
    int r = E_OK;
    switch(node->node_type) {
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
    case AST_NODE_BINOP: {
        type_t left;
        type_t right;
        r = get_expression_type(node->binop.left, &left);
        if(r != E_OK) {
            return r;
        }
        r = get_expression_type(node->binop.right, &right);
        if(r != E_OK) {
            return r;
        }
        r = get_binop_type(left, right, type);
        PRINT(3, "Get binop type: %s\n", type_to_readable(*type));
        if(r != E_OK) {
            return r;
        }
    } break;
    case AST_NODE_UNOP:
        *type = node->unop.metadata.result;
        break;
    case AST_NODE_SYMBOL:
        if(node->symbol.is_declaration) {
            PRINT(3, "ERROR: symbol not processed\n");
            // int r = check_variable(node,);
            //            if(r != E_OK) {
            //                return r;
            //            }
        }
        *type = node->symbol.declaration->type;
        break;
    case AST_NODE_FUNC_CALL:
        r = sem_get_func_call_type(node, type);
        if(r != E_OK) {
            return r;
        }
        break;
    default:
        // error?
        PRINT(3, "%s: Warn: unknown ast_node_type: %s\n", __func__,
                node_type_to_readable(node->node_type));
        break;
    }

    return E_OK;
}
*/

bool check_pass_type_compatibility(type_t source, type_t dest)
{
    if(source == TYPE_NIL) {
        return true;
    } else if(source == TYPE_INTEGER && dest == TYPE_NUMBER) {
        // INT2FLOAT
        return true;
    } else if(source == dest) {
        return true;
    }
    return false;
}

int check_func_call(ast_node_t *node, bool main_body)
{
    char *name = node->func_call.name.ptr;
    PRINT(3, "Check func call: %s\n", name);

    ast_node_t *sym = symtable_find_in_global(name);
    if(!sym) {
        PRINT(3, "SEM Error: function not defined\n");
        return E_UNDEF;
    }

    ast_func_def_t *def = get_func_def_ref(sym);
    node->func_call.def = def;

    if(main_body) {
        PRINT(3, "Checking in main body\n");

        // check return list

        ast_node_list_t return_types = get_func_return_types(sym);
        if(return_types) {
            PRINT(3, "SEM Error: function in main body can't return values\n");
            return E_TYPE_CALL;
        }
    }

    // special case: variadic arguments
    bool ignore_types = (strcmp(name, "write") == 0);
    if(ignore_types) {
        // check argument list
        for(ast_node_t *it = node->func_call.arguments; it; it = it->next) {
            type_t type;
            int r = check_expression(&it, &type);
            if(r != E_OK) {
                return r;
            }
        }

    } else {
        // check arguments and type compatibility
        ast_node_t *args = def->arguments;
        ast_node_t *input = node->func_call.arguments;
        while(input && args) {
            type_t source;
            int r = check_expression(&input, &source);
            if(r != E_OK) {
                return r;
            }

            type_t dest;
            r = sem_get_type(args, &dest);
            if(r != E_OK) {
                return r;
            }

            PRINT(3, "Checking argument types: %s vs %s\n", type_to_readable(source),
                  type_to_readable(dest));

            if(!check_pass_type_compatibility(source, dest)) {
                return E_TYPE_CALL;
            }

            input = input->next;
            args = args->next;
        }
        if(input || args) {
            PRINT(3, "SEM Error: wrong number of function call parameters\n");
            return E_TYPE_CALL;
        }
    }

    return E_OK;
}

static int compare_types(ast_node_list_t left, ast_node_list_t right)
{
    while(left && right) {
        // TODO
        type_t ltype;
        int r = sem_get_type(left, &ltype);
        if(r != E_OK) {
            return r;
        }
        type_t rtype;
        r = sem_get_type(right, &rtype);
        if(r != E_OK) {
            return r;
        }

        PRINT(3, "Checking types: %s vs %s\n", type_to_readable(ltype), type_to_readable(rtype));

        if(ltype != rtype) {
            PRINT(3, "SEM Error: function types mismatch\n");
            return E_SEM;
        }

        right = right->next;
        left = left->next;
    }
    if(left || right) {
        return E_SEM;
    }
    return E_OK;
}

static int check_declared_variable(char *identifier, ast_node_t *node)
{
    PRINT(3, "Check arg name: %s\n", identifier);
    ast_node_t *sym = symtable_find_in_current(identifier);
    if(sym) {
        PRINT(3, "SEM error: variable redeclaration\n");
        return E_REDEF;
    }
    ast_node_t *func = symtable_find_in_global(identifier);
    if(func) {
        PRINT(3, "SEM error: func with same name\n");
        return E_REDEF;
    }
    PRINT(3, "Adding to current scope\n");
    int r = symtable_put_symbol(identifier, node);
    if(r != E_OK) {
        return r;
    }

    static const char separator = '%';

    char temp[80];
    sprintf(temp, "%c%d", separator, symtable_scope_level());

    for(char *c = temp; *c != '\0'; ++c) {
        if(str_append_char(&node->declaration.symbol.name, *c) != 0) {
            return E_INT;
        }
    }

    return r;
}

static int check_argument_names(ast_node_t *node)
{
    symtable_push_scope(); // func_def body
    PRINT(3, "Checking definition arguments\n");
    ast_node_t *it = node->func_decl.argument_types;
    while(it) {
        char *name = it->symbol.name.ptr;
        int r = check_declared_variable(name, it);
        if(r != E_OK) {
            return r;
        }
        it = it->next;
    }
    return E_OK;
}

int check_function(ast_node_t *node, char *name)
{
    PRINT(3, "Checking funcion: %s\n", name);
    ast_node_t *sym = symtable_find_in_global(name);

    if(sym) {
        PRINT(3, "Symbol found\n");

        ast_func_decl_t *decl = get_func_decl_ref(sym);
        ast_func_def_t *def = get_func_def_ref(sym);
        if(node->node_type == AST_NODE_FUNC_DECL) {
            if(decl) {
                PRINT(3, "Error: duplicate declaration\n");
                return E_REDEF;
            } else {
                sym->func_def.decl = &node->func_decl;
                decl = &node->func_decl;
            }
        } else { // AST_NODE_FUNC_DEF
            if(def) {
                PRINT(3, "Error: duplicate definition\n");
                return E_REDEF;
            } else {
                sym->func_decl.def = &node->func_def;
                def = &node->func_def;
            }
        }

        if(decl && def) {
            int r = compare_types(decl->argument_types, def->arguments);
            if(r != E_OK) {
                return r;
            }
            r = compare_types(decl->return_types, def->return_types);
            if(r != E_OK) {
                return r;
            }
        }

    } else {
        PRINT(3, "Adding func to symtable\n");
        if(symtable_put_in_global(name, node) != E_OK) {
            return E_INT;
        }
    }

    if(node->node_type == AST_NODE_FUNC_DEF) {
        int r = check_argument_names(node);
        if(r != E_OK) {
            return r;
        }
    }
    return E_OK;
}

int sem_check(ast_node_t *node, int i, nut_type_t expected)
{
    (void) i;
    if(node) {

        const char *s;
        (void) s;
        if(expected.is_nterm) {
            s = nterm_to_readable(expected.nterm);
        } else {
            s = term_to_readable(expected.term);
            if(expected.term == T_END) {
                PRINT(3, "SEM: Popping scope\n");
                if(symtable_pop_scope() != E_OK) {
                    return E_INT;
                }
            }
        }
        PRINT(2, "    Semantic check: %s %d %s\n", s, i, node_type_to_readable(node->node_type));

        switch(node->node_type) {
            //        case AST_NODE_BODY:
            //            if(expected.is_nterm && expected.nterm == NT_STATEMENT_LIST2) {

            //            } else if(expected.is_nterm && expected.nterm == NT_STATEMENT) {
            //                //                PRINT(3, "SEM: pushing scope\n");
            //                //                if(symtable_push_scope() != E_OK) {
            //                //                    return E_INT;
            //                //                }
            //            }
            //            break;
        case AST_NODE_PROGRAM:
            if(!expected.is_nterm && expected.term == T_STRING) {
                if(strcmp(node->program.require.ptr, "ifj21") != 0) {
                    fprintf(stderr, "SEM Error: wrong preamble\n");
                    return E_SEM;
                }
            }
            break;
        case AST_NODE_WHILE:
            if(!expected.is_nterm && expected.term == T_DO) {

                if(symtable_push_scope() != E_OK) {
                    return E_INT;
                }

                type_t type;
                int r = check_expression(&node->while_loop.condition, &type);
                if(r != E_OK) {
                    return r;
                }
            }
            break;
        case AST_NODE_FOR:
            if(!expected.is_nterm && (expected.term == T_DO)) {

                if(symtable_push_scope() != E_OK) {
                    return E_INT;
                }

                if(str_append_char(&node->for_loop.iterator->symbol.name, '_') != E_OK) {
                    return E_INT;
                }

                int r = check_declared_variable(node->for_loop.iterator->symbol.name.ptr,
                                                node->for_loop.iterator);
                if(r != E_OK) {
                    return r;
                }

                type_t type_setup;
                r = check_expression(&node->for_loop.setup, &type_setup);
                if(r != E_OK) {
                    return r;
                }

                type_t type_condition;
                r = check_expression(&node->for_loop.condition, &type_condition);
                if(r != E_OK) {
                    return r;
                }

                if(node->for_loop.step) {
                    type_t type_step;
                    r = check_expression(&node->for_loop.step, &type_step);
                    if(r != E_OK) {
                        return r;
                    }
                }
            }
            break;
        case AST_NODE_IF:

            if(!expected.is_nterm && (expected.term == T_ELSEIF || expected.term == T_ELSE)) {
                PRINT(3, "SEM: popping scope (IF)\n");
                if(symtable_pop_scope() != E_OK) {
                    return E_INT;
                }
            }
            if(!expected.is_nterm && (expected.term == T_THEN || expected.term == T_ELSE)) {
                PRINT(3, "SEM: pushing scope (IF)\n");
                if(symtable_push_scope() != E_OK) {
                    return E_INT;
                }
            }
            if(!expected.is_nterm && expected.term == T_END) {
                PRINT(3, "SEM: check if expression\n");
                ast_node_t *cond = node->if_condition.conditions;
                while(cond) {
                    type_t source;
                    int r = check_expression(&cond, &source);
                    if(r != E_OK) {
                        return r;
                    }
                    cond = cond->next;
                }
            }
            break;
        case AST_NODE_ASSIGNMENT:
            if(expected.is_nterm && expected.nterm == NT_STATEMENT) {
                PRINT(3, "AST_NODE_ASSIGNMENT -> NT_STATEMENT\n");
            } else if(expected.is_nterm && expected.nterm == NT_EXPRESSION_LIST) {
                PRINT(3, "Check assignment:\n");

                ast_node_t *ids = node->assignment.identifiers;
                ast_node_t *exp = node->assignment.expressions;
                while(ids) {
                    int r = check_variable(ids, false, true);
                    if(r != E_OK) {
                        return r;
                    }

                    if(exp) {
                        type_t source;
                        // PRINT(3, " >> GET TYPE START\n");
                        int r = check_expression(&exp, &source);
                        // PRINT(3, " >> GET TYPE END\n");
                        PRINT(3, "Get type (R%d): %s\n", r, type_to_readable(source));
                        if(r != E_OK) {
                            return r;
                        }
                        type_t dest = ids->symbol.declaration->type;
                        if(!check_pass_type_compatibility(source, dest)) {
                            return E_ASSIGN;
                        }
                        exp = exp->next;
                    }
                    ids = ids->next;
                }

                PRINT(3, "Check ~~ assignment types:\n");
            }
            break;
        case AST_NODE_DECLARATION:
            if(expected.is_nterm && expected.nterm == NT_DECLARATION) {

                char *name = node->declaration.symbol.name.ptr;

                PRINT(3, "                  check local declaration: %s\n", name);

                int r = E_OK;
                if(node->declaration.assignment) {
                    PRINT(3, "  check declaration assignment\n");
                    type_t source;
                    r = check_expression(&node->declaration.assignment, &source);
                    if(r != E_OK) {
                        return r;
                    }
                    type_t dest = node->declaration.symbol.type;
                    if(!check_pass_type_compatibility(source, dest)) {
                        return E_ASSIGN;
                    }
                }

                r = check_declared_variable(name, node);
                if(r != E_OK) {
                    return r;
                }
            }
            break;
        case AST_NODE_FUNC_DECL:
            if(expected.is_nterm && expected.nterm == NT_FUNC_DECL) {
                char *name = node->func_decl.name.ptr;
                int r = check_function(node, name);
                if(r != E_OK) {
                    return r;
                }
            }
            break;
        case AST_NODE_FUNC_DEF:
            if(expected.is_nterm && expected.nterm == NT_FUNC_TYPE_LIST) {

                char *name = node->func_def.name.ptr;

                int r = check_function(node, name);
                if(r != E_OK) {
                    return r;
                }

                current_def = &node->func_def;
            }
            break;
        case AST_NODE_FUNC_CALL:
            if(expected.is_nterm && expected.nterm == NT_GLOBAL_STATEMENT) {
                int r = check_func_call(node, true);
                if(r != E_OK) {
                    return r;
                }
            } else if(expected.is_nterm && expected.nterm == NT_PAREN_EXP_LIST_OR_ID_LIST2) {
                PRINT(3, "Checking f here\n");
                int r = check_func_call(node, false);
                if(r != E_OK) {
                    return r;
                }
            }
            break;
        case AST_NODE_RETURN:
            if(expected.is_nterm && expected.nterm == NT_RET_EXPRESSION_LIST) {
                // char *name = node->func_call.name.ptr;
                PRINT(3, "%s: checking return in: %s\n", __func__, current_def->name.ptr);

                node->return_values.def = current_def;

                ast_node_t *values = node->return_values.values;
                while(values) {
                    type_t t1;
                    int r = check_expression(&values, &t1);
                    if(r != E_OK) {
                        return r;
                    }

                    values = values->next;
                }

                values = node->return_values.values;
                ast_node_t *types = current_def->return_types;

                while(values && types) {

                    PRINT(3, "  checking %s against %s\n", node_type_to_readable(values->node_type),
                          node_type_to_readable(types->node_type));

                    type_t t1;
                    //                    if(values->node_type == AST_NODE_FUNC_CALL &&
                    //                    !values->next) {
                    //                        ast_func_def_t *def = values->func_call.def;
                    //                        PRINT(3, "   adjusting [%p]\n", def);
                    //                        if(def) {
                    //                            values = def->return_types;
                    //                        }
                    //                    } else if(values->node_type == AST_NODE_FUNC_CALL) {
                    //                        int r = sem_get_func_call_type(values, &t1);
                    //                        if(r != E_OK) {
                    //                            return r;
                    //                        }
                    //                    }

                    int r = sem_get_type(values, &t1);
                    if(r != E_OK) {
                        return r;
                    }

                    type_t t2;
                    r = sem_get_type(types, &t2);
                    if(r != E_OK) {
                        return r;
                    }
                    PRINT(3, "%s vs %s\n", type_to_readable(t1), type_to_readable(t2));
                    if(!check_pass_type_compatibility(t1, t2)) {
                        return E_TYPE_CALL;
                    }

                    values = values->next;
                    types = types->next;
                }
                if(values) {
                    PRINT(3, "SEM Error: returning more values\n");
                    return E_TYPE_CALL;
                }
            }
            break;

        default:
            break;
        }
    }

    return E_OK;
}

int check_binop_node(ast_node_t **node, type_t *type);
int check_unop_node(ast_node_t **node, type_t *type);

static int check_expression(ast_node_t **node, type_t *type)
{
    PRINT(3, "SEM: Checking expression\n");
    if(!node) {
        PRINT(3, "Warn: null node\n");
        return E_INT;
    }

    PRINT(3, "  Node: %s\n", node_type_to_readable((*node)->node_type));

    int r = E_OK;
    switch((*node)->node_type) {
    case AST_NODE_BINOP:
        r = check_binop_node(node, type);
        break;
    case AST_NODE_UNOP:
        r = check_unop_node(node, type);
        break;
    case AST_NODE_SYMBOL:
        r = check_variable(*node, false, false);
        if(r != E_OK) {
            return r;
        }
        // (*node)->symbol.declaration->used = true;
        *type = (*node)->symbol.declaration->type;
        break;
    case AST_NODE_FUNC_CALL:
        if((*node)->func_call.def) {
            PRINT(3, "Func in expr already checked\n");
        } else {
            PRINT(3, "Check func call in expr\n");
        }
        r = check_func_call(*node, false);
        if(r != E_OK) {
            return r;
        }
        r = sem_get_func_call_type(*node, type);
        if(r != E_OK) {
            return r;
        }
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
        r = E_INT;
        PRINT(3, "[SEM] %s: got unknown node_type\n", __func__);
        break;
    }
    return r;
}

void exec_factor(ast_node_t *node)
{
    PRINT(3, "%s", node_type_to_readable(node->node_type));
    if(node->node_type == AST_NODE_INTEGER) {
        PRINT(3, " int: %ld", node->integer);
    }
    PRINT(3, "\n");
}

void exec_node(ast_node_t *node)
{
    PRINT(3, "exec\n");
    PRINT(3, "OP left: ");
    exec_factor(node->binop.left);
    PRINT(3, "OP right: ");
    exec_factor(node->binop.right);
    PRINT(3, "Operation: %d\n", node->binop.type);
}

bool check_unop_operation(ast_node_unop_type_t type, type_t result)
{
    switch(type) {
    case AST_NODE_UNOP_LEN:
        return result == TYPE_STRING;
    case AST_NODE_UNOP_NEG:
        return is_number_or_integer(result);
    case AST_NODE_UNOP_NOT:
        return result == TYPE_BOOL;
    }
    return false;
}

int check_unop_node(ast_node_t **node, type_t *type)
{
    type_t optype;
    int r = check_expression(&(*node)->unop.operand, &optype);
    if(r != E_OK) {
        return r;
    }

    if(!check_unop_operation((*node)->unop.type, optype)) {
        return E_TYPE_EXPR;
    }
    switch((*node)->unop.type) {
    case AST_NODE_UNOP_LEN:
        (*node)->unop.metadata.result = TYPE_INTEGER;
        break;
    case AST_NODE_UNOP_NEG:
        (*node)->unop.metadata.result = optype;
        break;
    case AST_NODE_UNOP_NOT:
        (*node)->unop.metadata.result = TYPE_BOOL;
        break;
    }

    *type = (*node)->unop.metadata.result;

    return r;
}

bool check_binop_operation(ast_node_binop_type_t type, type_t source, type_t *result)
{
    *result = source;
    switch(type) {
    case AST_NODE_BINOP_ADD:
    case AST_NODE_BINOP_SUB:
    case AST_NODE_BINOP_MUL:
    case AST_NODE_BINOP_DIV:
    case AST_NODE_BINOP_MOD:
    case AST_NODE_BINOP_POWER:
        return is_number_or_integer(source);
    case AST_NODE_BINOP_INTDIV:
        return source == TYPE_INTEGER;
    case AST_NODE_BINOP_AND:
    case AST_NODE_BINOP_OR:
        return source == TYPE_BOOL;
    case AST_NODE_BINOP_LT:
    case AST_NODE_BINOP_LTE:
    case AST_NODE_BINOP_GT:
    case AST_NODE_BINOP_GTE:
    case AST_NODE_BINOP_EQ:
    case AST_NODE_BINOP_NE:
        *result = TYPE_BOOL;
        return source == TYPE_NUMBER || source == TYPE_INTEGER || source == TYPE_STRING ||
               source == TYPE_BOOL;
    case AST_NODE_BINOP_CONCAT:
        return source == TYPE_STRING;
    }
    return false;
}

int check_binop_node(ast_node_t **node, type_t *type)
{
    int r;

    type_t left;
    r = check_expression(&(*node)->binop.left, &left);
    if(r != E_OK) {
        return r;
    }
    type_t right;
    r = check_expression(&(*node)->binop.right, &right);
    if(r != E_OK) {
        return r;
    }

    bool type_check_skip = false;
    if((*node)->binop.type == AST_NODE_BINOP_EQ || (*node)->binop.type == AST_NODE_BINOP_NE) {
        if(left == TYPE_NIL || right == TYPE_NIL) {
            type_check_skip = true;
            *type = TYPE_BOOL;
        }
    }

    if(!type_check_skip) {

        r = get_binop_type(left, right, type);
        PRINT(3, "[r: %d]Eval binop node: %d   %s\n", r, (*node)->binop.type,
              type_to_readable(*type));
        if(r != E_OK) {
            return r;
        }
        if(!check_binop_operation((*node)->binop.type, *type, type)) {
            return E_TYPE_EXPR;
        }
    }

    (*node)->binop.metadata.result = *type;

    return r;
}
/*
int sem_check_expression(ast_node_t *node)
{
    type_t type;
    int r = check_expression(node, &type);

    PRINT(3, "RESULT type[%d]: %s\n", r, type_to_readable(type));
    return r;
}
*/
static ast_node_t builtin_write = { .node_type = AST_NODE_FUNC_DEF,
                                    .func_def = { .name.ptr = "write",
                                                  .arguments = NULL,
                                                  .return_types = NULL,
                                                  .body = NULL,
                                                  .decl = NULL },
                                    .next = NULL,
                                    .visited_children = 0 };

static ast_node_t builtin_reads = { .node_type = AST_NODE_FUNC_DEF,
                                    .func_def = { .name.ptr = "reads",
                                                  .arguments = NULL,
                                                  .return_types =
                                                      &(ast_node_t){ .node_type = AST_NODE_TYPE,
                                                                     .type = TYPE_STRING,
                                                                     .next = NULL,
                                                                     .visited_children = 0 },
                                                  .body = NULL,
                                                  .decl = NULL },
                                    .next = NULL,
                                    .visited_children = 0 };

static ast_node_t builtin_readi = { .node_type = AST_NODE_FUNC_DEF,
                                    .func_def = { .name.ptr = "readi",
                                                  .arguments = NULL,
                                                  .return_types =
                                                      &(ast_node_t){ .node_type = AST_NODE_TYPE,
                                                                     .type = TYPE_INTEGER,
                                                                     .next = NULL,
                                                                     .visited_children = 0 },
                                                  .body = NULL,
                                                  .decl = NULL },
                                    .next = NULL,
                                    .visited_children = 0 };

static ast_node_t builtin_readn = { .node_type = AST_NODE_FUNC_DEF,
                                    .func_def = { .name.ptr = "readn",
                                                  .arguments = NULL,
                                                  .return_types =
                                                      &(ast_node_t){ .node_type = AST_NODE_TYPE,
                                                                     .type = TYPE_NUMBER,
                                                                     .next = NULL,
                                                                     .visited_children = 0 },
                                                  .body = NULL,
                                                  .decl = NULL },
                                    .next = NULL,
                                    .visited_children = 0 };

static ast_node_t builtin_tointeger = {
    .node_type = AST_NODE_FUNC_DEF,
    .func_def = { .name.ptr = "tointeger",
                  .arguments = &(ast_node_t){ .node_type = AST_NODE_SYMBOL,
                                              .symbol.type = TYPE_NUMBER,
                                              .next = NULL,
                                              .visited_children = 0 },
                  .return_types = &(ast_node_t){ .node_type = AST_NODE_TYPE,
                                                 .type = TYPE_INTEGER,
                                                 .next = NULL,
                                                 .visited_children = 0 },
                  .body = NULL,
                  .decl = NULL },
    .next = NULL,
    .visited_children = 0
};

static ast_node_t builtin_substr = {
    .node_type = AST_NODE_FUNC_DEF,
    .func_def = { .name.ptr = "substr",
                  .arguments =
                      &(ast_node_t){ .node_type = AST_NODE_SYMBOL,
                                     .symbol.type = TYPE_STRING,
                                     .visited_children = 0,
                                     .next =
                                         &(ast_node_t){
                                             .node_type = AST_NODE_SYMBOL,
                                             .symbol.type = TYPE_NUMBER,
                                             .visited_children = 0,
                                             .next = &(ast_node_t){ .node_type = AST_NODE_SYMBOL,
                                                                    .symbol.type = TYPE_NUMBER,
                                                                    .next = NULL,
                                                                    .visited_children = 0 },
                                         } },
                  .return_types = &(ast_node_t){ .node_type = AST_NODE_TYPE,
                                                 .type = TYPE_STRING,
                                                 .next = NULL,
                                                 .visited_children = 0 },
                  .body = NULL,
                  .decl = NULL },
    .next = NULL,
    .visited_children = 0
};

static ast_node_t builtin_ord = {
    .node_type = AST_NODE_FUNC_DEF,
    .func_def = { .name.ptr = "ord",
                  .arguments = &(ast_node_t){ .node_type = AST_NODE_SYMBOL,
                                              .symbol.type = TYPE_STRING,
                                              .visited_children = 0,
                                              .next = &(ast_node_t){ .node_type = AST_NODE_SYMBOL,
                                                                     .symbol.type = TYPE_INTEGER,
                                                                     .next = NULL,
                                                                     .visited_children = 0 } },
                  .return_types = &(ast_node_t){ .node_type = AST_NODE_TYPE,
                                                 .type = TYPE_INTEGER,
                                                 .next = NULL,
                                                 .visited_children = 0 },
                  .body = NULL,
                  .decl = NULL },
    .next = NULL,
    .visited_children = 0
};

static ast_node_t builtin_chr = {
    .node_type = AST_NODE_FUNC_DEF,
    .func_def = { .name.ptr = "chr",
                  .arguments = &(ast_node_t){ .node_type = AST_NODE_SYMBOL,
                                              .symbol.type = TYPE_INTEGER,
                                              .next = NULL,
                                              .visited_children = 0 },
                  .return_types = &(ast_node_t){ .node_type = AST_NODE_TYPE,
                                                 .type = TYPE_STRING,
                                                 .next = NULL,
                                                 .visited_children = 0 },
                  .body = NULL,
                  .decl = NULL },
    .next = NULL,
    .visited_children = 0
};

int sem_init()
{
    int r = symtable_init();
    if(r != E_OK) {
        return r;
    }

    symtable_put_in_global("write", &builtin_write);
    symtable_put_in_global("reads", &builtin_reads);
    symtable_put_in_global("readi", &builtin_readi);
    symtable_put_in_global("readn", &builtin_readn);
    symtable_put_in_global("tointeger", &builtin_tointeger);
    symtable_put_in_global("substr", &builtin_substr);
    symtable_put_in_global("ord", &builtin_ord);
    symtable_put_in_global("chr", &builtin_chr);

    current_def = NULL;

    return E_OK;
}

void sem_free()
{
    symtable_free();
}
