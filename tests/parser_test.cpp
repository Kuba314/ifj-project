#include <stdexcept>

#include <gtest/gtest.h>
extern "C" {
#include <stdarg.h>
#include "parser.h"
#include "scanner.h"
#include "semantics.h"
#include "error.h"
}

const char *parser_exception_to_string(int err)
{
    switch(err) {
    case E_OK:
        return "success";
    case E_LEX:
        return "scanner error";
    case E_SYN:
        return "syntax error";
    case E_UNDEF:
        return "undefined / redefined symbol";
    case E_ASSIGN:
        return "assignment type incompatibility";
    case E_TYPE_CALL:
        return "invalid number / type of function arguments or return values";
    case E_TYPE_EXPR:
        return "invalid type for operator in expression";
    case E_SEM:
        return "semantic error";
    case E_NIL:
        return "unexpected nil";
    case E_ZERODIV:
        return "division by zero";
    case E_INT:
        return "internal error";
    default:
        return "unknown error";
    }
}

class ParserTests : public ::testing::Test {
  protected:
    void InitTest(const char *filename)
    {
        FILE *fp = fopen(filename, "r");
        if(fp == nullptr) {
            perror("InitTest");
        }

        // throw exception if allocation failed
        scanner_init(fp);
        if(parser_init()) {
            throw std::bad_alloc();
        }

        if(semantics_init()) {
            throw std::bad_alloc();
        }

        int err = parse(NT_PROGRAM, &ast, 0);
        if(err) {
            std::cerr << "Failed to parse: " << parser_exception_to_string(err) << std::endl;
            throw std::exception();
        }
        print_ast(0, ast);
    }
    virtual void TearDown() override
    {
        free_ast(ast);
        semantics_free();
        parser_free();
        scanner_free();
    }

    ast_node_t *ast = NULL;
};

// these have to be macros, because ASSERT_ doesn't exit, it just returns
#define check_str_node(node, str)                                                                  \
    ASSERT_NE(node, nullptr);                                                                      \
    EXPECT_EQ(node->node_type, AST_NODE_STRING);                                                   \
    EXPECT_EQ(strcmp(node->string.ptr, str), 0);

#define check_sym_node(node, str)                                                                  \
    ASSERT_NE(node, nullptr);                                                                      \
    EXPECT_EQ(node->node_type, AST_NODE_SYMBOL);                                                   \
    EXPECT_EQ(strcmp(node->symbol.is_declaration ? node->symbol.name.ptr                           \
                                                 : node->symbol.declaration->name.ptr,             \
                     str),                                                                         \
              0);

// node_type has to be asserted, because usually nodes have children and this wouldn't end well
#define check_node(node, type)                                                                     \
    ASSERT_NE(node, nullptr);                                                                      \
    ASSERT_EQ(node->node_type, type);

#define check_str(str, c_str) EXPECT_EQ(strcmp(str.ptr, c_str), 0);

void check_arg_names(ast_func_def_t func_def, ...)
{
    va_list args;
    va_start(args, func_def);

    ast_node_list_t arg_list = func_def.arguments;
    const char *arg_name = va_arg(args, const char *);
    while(arg_list != nullptr || arg_name != nullptr) {
        if(arg_list == nullptr) {
            ASSERT_NE(arg_list, nullptr);
        } else if(arg_name == nullptr) {
            ASSERT_NE(arg_name, nullptr);
        }

        EXPECT_EQ(arg_list->node_type, AST_NODE_SYMBOL);
        EXPECT_TRUE(arg_list->symbol.is_declaration);
        EXPECT_EQ(strcmp(arg_list->symbol.name.ptr, arg_name), 0);

        arg_list = arg_list->next;
        arg_name = va_arg(args, const char *);
    }

    va_end(args);
}
void check_arg_types(ast_func_def_t func_def, ...)
{
    va_list args;
    va_start(args, func_def);

    ast_node_list_t arg_list = func_def.arguments;
    type_t arg_type = (type_t) va_arg(args, int);
    while(arg_list != nullptr || arg_type != TYPE_NIL) {
        if(arg_list == nullptr) {
            ASSERT_NE(arg_list, nullptr);
        } else if(arg_type == TYPE_NIL) {
            ASSERT_NE(arg_type, TYPE_NIL);
        }

        EXPECT_EQ(arg_list->node_type, AST_NODE_SYMBOL);
        EXPECT_TRUE(arg_list->symbol.is_declaration);
        EXPECT_EQ(arg_list->symbol.type, arg_type);

        arg_list = arg_list->next;
        arg_type = (type_t) va_arg(args, int);
    }

    va_end(args);
}
void check_type_list(ast_node_t *type_it, ...)
{
    va_list args;
    va_start(args, type_it);

    type_t arg_type = (type_t) va_arg(args, int);
    while(type_it != nullptr || arg_type != TYPE_NIL) {
        if(type_it == nullptr) {
            ASSERT_NE(type_it, nullptr);
        } else if(arg_type == TYPE_NIL) {
            ASSERT_NE(arg_type, TYPE_NIL);
        }

        EXPECT_EQ(type_it->node_type, AST_NODE_TYPE);
        EXPECT_EQ(type_it->type, arg_type);

        type_it = type_it->next;
        arg_type = (type_t) va_arg(args, int);
    }

    va_end(args);
}

TEST_F(ParserTests, Add)
{
    InitTest("tests/test_files/add.tl");

    EXPECT_EQ(ast->node_type, AST_NODE_PROGRAM);
    ast_node_t *global_it = ast->program.global_statement_list;

    check_node(global_it, AST_NODE_FUNC_DEF);
    ast_func_def_t func_def = global_it->func_def;

    EXPECT_EQ(strcmp(func_def.name.ptr, "add"), 0);
    check_arg_names(func_def, "a%1", "b%1", NULL);
    check_arg_types(func_def, TYPE_INTEGER, TYPE_INTEGER, TYPE_NIL);
    check_type_list(func_def.return_types, TYPE_INTEGER, TYPE_NIL);

    ASSERT_NE(func_def.body, nullptr);
    EXPECT_EQ(func_def.body->node_type, AST_NODE_BODY);
    ast_node_t *statement_it = func_def.body->body.statements;

    check_node(statement_it, AST_NODE_RETURN);
    ast_node_list_t ret_values_it = statement_it->return_values.values;

    check_node(ret_values_it, AST_NODE_BINOP);
    EXPECT_EQ(ret_values_it->binop.type, AST_NODE_BINOP_ADD);
    check_node(ret_values_it->binop.left, AST_NODE_SYMBOL);
    check_node(ret_values_it->binop.right, AST_NODE_SYMBOL);
    check_sym_node(ret_values_it->binop.left, "a%1");
    check_sym_node(ret_values_it->binop.right, "b%1");

    global_it = global_it->next;

    ASSERT_NE(global_it, nullptr);

    ast_func_def_t func_def2 = global_it->func_def;

    check_str(func_def2.name, "add_local");
    check_arg_names(func_def2, "a%1", "b%1", NULL);
    check_arg_types(func_def2, TYPE_INTEGER, TYPE_INTEGER, TYPE_NIL);
    check_type_list(func_def2.return_types, TYPE_INTEGER, TYPE_NIL);

    ASSERT_NE(func_def2.body, nullptr);
    EXPECT_EQ(func_def2.body->node_type, AST_NODE_BODY);
    ast_node_t *statement_it2 = func_def2.body->body.statements;

    check_node(statement_it2, AST_NODE_DECLARATION);
    EXPECT_EQ(statement_it2->declaration.symbol.type, TYPE_INTEGER);
    check_str(statement_it2->declaration.symbol.name, "c%1");
    check_node(statement_it2->declaration.assignment, AST_NODE_BINOP);
    EXPECT_EQ(statement_it2->declaration.assignment->binop.type, AST_NODE_BINOP_ADD);
    check_node(statement_it2->declaration.assignment->binop.left, AST_NODE_SYMBOL);
    check_node(statement_it2->declaration.assignment->binop.right, AST_NODE_SYMBOL);
    check_sym_node(statement_it2->declaration.assignment->binop.left, "a%1");
    check_sym_node(statement_it2->declaration.assignment->binop.right, "b%1");

    statement_it2 = statement_it2->next;

    check_node(statement_it2, AST_NODE_RETURN);
    ast_node_list_t ret_values_it2 = statement_it2->return_values.values;

    check_sym_node(ret_values_it2, "c%1");

    EXPECT_EQ(ret_values_it2->next, nullptr);
    EXPECT_EQ(statement_it2->next, nullptr);
}
TEST_F(ParserTests, Pad)
{
    InitTest("tests/test_files/pad.tl");

    EXPECT_EQ(ast->node_type, AST_NODE_PROGRAM);
    ast_node_t *global_it = ast->program.global_statement_list;

    check_node(global_it, AST_NODE_FUNC_DEF);
    ast_func_def_t func_def = global_it->func_def;

    check_str(func_def.name, "pad");
    check_arg_names(func_def, "str%1", "n_spaces%1", NULL);
    check_arg_types(func_def, TYPE_STRING, TYPE_INTEGER, TYPE_NIL);
    check_type_list(func_def.return_types, TYPE_NIL);

    check_node(func_def.body, AST_NODE_BODY);
    ast_node_t *statement_it = func_def.body->body.statements;

    check_node(statement_it, AST_NODE_WHILE);
    ast_node_t *condition = statement_it->while_loop.condition;
    check_node(condition, AST_NODE_BINOP);

    EXPECT_EQ(condition->binop.type, AST_NODE_BINOP_GT);
    check_node(condition->binop.left, AST_NODE_SYMBOL);
    check_node(condition->binop.right, AST_NODE_INTEGER);
    check_sym_node(condition->binop.left, "n_spaces%1");
    ASSERT_EQ(condition->binop.right->integer, 0);

    check_node(statement_it->while_loop.body, AST_NODE_BODY);
    ast_node_t *while_stmt_it = statement_it->while_loop.body->body.statements;
    check_node(while_stmt_it, AST_NODE_FUNC_CALL);
    check_str(while_stmt_it->func_call.name, "write");
    ast_node_t *arg_it = while_stmt_it->func_call.arguments;
    check_str_node(arg_it, " ");
    EXPECT_EQ(arg_it->next, nullptr);

    while_stmt_it = while_stmt_it->next;
    check_node(while_stmt_it, AST_NODE_ASSIGNMENT);
    ast_node_t *id_it = while_stmt_it->assignment.identifiers;
    check_sym_node(id_it, "n_spaces%1");
    EXPECT_EQ(id_it->next, nullptr);

    ast_node_t *expr_it = while_stmt_it->assignment.expressions;
    check_node(expr_it, AST_NODE_BINOP);

    EXPECT_EQ(expr_it->binop.type, AST_NODE_BINOP_SUB);
    check_node(expr_it->binop.left, AST_NODE_SYMBOL);
    check_node(expr_it->binop.right, AST_NODE_INTEGER);
    check_sym_node(expr_it->binop.left, "n_spaces%1");
    ASSERT_EQ(expr_it->binop.right->integer, 1);
    EXPECT_EQ(expr_it->next, nullptr);
    EXPECT_EQ(while_stmt_it->next, nullptr);

    statement_it = statement_it->next;

    check_node(statement_it, AST_NODE_FUNC_CALL);
    check_str(statement_it->func_call.name, "write");
    ast_node_t *arg2_it = statement_it->func_call.arguments;
    check_node(arg2_it, AST_NODE_SYMBOL);
    check_sym_node(arg2_it, "str%1");
    EXPECT_EQ(arg2_it->next, nullptr);
}
TEST_F(ParserTests, HelloWorld)
{
    InitTest("tests/test_files/hello_world.tl");

    EXPECT_EQ(ast->node_type, AST_NODE_PROGRAM);
    ast_node_t *global_it = ast->program.global_statement_list;

    check_node(global_it, AST_NODE_FUNC_DECL);
    ast_func_decl_t func_decl = global_it->func_decl;

    check_type_list(func_decl.argument_types, TYPE_NIL);
    check_type_list(func_decl.return_types, TYPE_NIL);

    global_it = global_it->next;

    check_node(global_it, AST_NODE_FUNC_CALL);
    check_str(global_it->func_call.name, "main");
    EXPECT_EQ(global_it->func_call.arguments, nullptr);

    global_it = global_it->next;

    ast_func_def_t func_def = global_it->func_def;

    check_str(func_def.name, "main");
    check_arg_names(func_def, NULL);
    check_arg_types(func_def, TYPE_NIL);
    check_type_list(func_def.return_types, TYPE_NIL);

    check_node(func_def.body, AST_NODE_BODY);
    ast_node_t *statement_it = func_def.body->body.statements;

    check_node(statement_it, AST_NODE_FUNC_CALL);
    check_str(statement_it->func_call.name, "write");

    ast_node_t *arg_it = statement_it->func_call.arguments;
    check_str_node(arg_it, "Hello world!\n");
    EXPECT_EQ(arg_it->next, nullptr);

    EXPECT_EQ(statement_it->next, nullptr);
    EXPECT_EQ(global_it->next, nullptr);
}
TEST_F(ParserTests, Return)
{
    InitTest("tests/test_files/returns.tl");
}
