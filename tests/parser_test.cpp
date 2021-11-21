#include <stdexcept>

#include <gtest/gtest.h>
extern "C" {
#include <stdarg.h>
#include "parser.h"
#include "scanner.h"
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
        parser_free();
        scanner_free();
    }

    ast_node_t *ast;
};

// these have to be macros, because ASSERT_ doesn't exit, it just returns
#define check_str(node, str) \
    ASSERT_NE(node, nullptr); \
    EXPECT_EQ(node->node_type, AST_NODE_STRING); \
    EXPECT_EQ(strcmp(node->string.ptr, str), 0);

#define check_id(node, str) \
    ASSERT_NE(node, nullptr); \
    EXPECT_EQ(node->node_type, AST_NODE_IDENTIFIER); \
    EXPECT_EQ(strcmp(node->identifier.ptr, str), 0);

// node_type has to be asserted, because usually nodes have children and this wouldn't end well
#define check_node(node, type) \
    ASSERT_NE(node, nullptr); \
    ASSERT_EQ(node->node_type, type);

void check_arg_names(ast_func_def_t func_def, ...)
{
    va_list args;
    va_start(args, func_def);

    ast_node_list_t arg_list = func_def.arguments;
    const char *arg_name = va_arg(args, const char *);
    while(arg_list != nullptr || arg_name != nullptr) {
        if(arg_list == nullptr) {
            EXPECT_NE(arg_list, nullptr);
        } else if(arg_name == nullptr) {
            EXPECT_NE(arg_name, nullptr);
        }

        EXPECT_EQ(arg_list->node_type, AST_NODE_ID_TYPE_PAIR);
        EXPECT_EQ(strcmp(arg_list->id_type_pair.id.ptr, arg_name), 0);

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
            EXPECT_NE(arg_list, nullptr);
        } else if(arg_type == TYPE_NIL) {
            EXPECT_NE(arg_type, TYPE_NIL);
        }

        EXPECT_EQ(arg_list->node_type, AST_NODE_ID_TYPE_PAIR);
        EXPECT_EQ(arg_list->id_type_pair.type, arg_type);

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
            EXPECT_NE(type_it, nullptr);
        } else if(arg_type == TYPE_NIL) {
            EXPECT_NE(arg_type, TYPE_NIL);
        }

        EXPECT_EQ(type_it->node_type, AST_NODE_TYPE);
        EXPECT_EQ(type_it->type, arg_type);

        type_it = type_it->next;
        arg_type = (type_t) va_arg(args, int);
    }

    va_end(args);
}

TEST_F(ParserTests, Negate)
{
    InitTest("tests/test_files/negate.tl");

    EXPECT_EQ(ast->node_type, AST_NODE_PROGRAM);
    ast_node_t *global_it = ast->program.global_statement_list;

    check_node(global_it, AST_NODE_FUNC_DEF);
    ast_func_def_t func_def = global_it->func_def;

    check_id(func_def.name, "negate");
    check_arg_names(func_def, "n", NULL);
    check_arg_types(func_def, TYPE_INTEGER, TYPE_NIL);
    check_type_list(func_def.return_types, TYPE_INTEGER, TYPE_NIL);

    ASSERT_NE(func_def.body, nullptr);
    EXPECT_EQ(func_def.body->node_type, AST_NODE_BODY);
    ast_node_t *statement_it = func_def.body->body.statements;

    check_node(statement_it, AST_NODE_RETURN);
    ast_node_list_t ret_values_it = statement_it->return_values.values;

    check_node(ret_values_it, AST_NODE_UNOP);
    EXPECT_EQ(ret_values_it->unop.type, AST_NODE_UNOP_NEG);
    ast_node_t *operand = ret_values_it->unop.operand;
    ASSERT_NE(operand, nullptr);
    EXPECT_EQ(ret_values_it->next, nullptr);

    check_id(operand, "n");
}
