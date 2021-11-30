#include <stdexcept>
#include <string.h>

#include <gtest/gtest.h>
#include <iostream>
extern "C" {
#include "scanner.h"
#include "type.h"
#include "error.h"
#include "dynstring.h"
#include "parser-generated.h"
}

class ScannerInput : public ::testing::Test {
  protected:
    virtual void UseFile(const char *filename)
    {
        FILE *f = fopen(filename, "r");
        scanner_init(f);
    }
    virtual void TearDown() override
    {
        scanner_free();
    }
};

TEST_F(ScannerInput, NumberScan)
{
    UseFile("tests/scanner_test_files/numbers.tl");
    token_t token;

    double numbers[] = { 3.14, 3.1, 3123.0000001, 30000.0, 3000.0, 42.4e3, 42.4e+3, 42.4e-3 };
    int pos_rows[] = { 2, 3, 4, 5, 6, 7, 8, 9 };

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 123);
    EXPECT_EQ(token.token_type, T_INTEGER);
    EXPECT_EQ(token.row, 1);
    EXPECT_EQ(token.column, 1);

    for(int i = 0; i < 8; i++) {
        ASSERT_EQ(get_next_token(&token), E_OK);
        EXPECT_EQ(token.number, (double) numbers[i]);
        EXPECT_EQ(token.token_type, T_NUMBER);
        EXPECT_EQ(token.row, pos_rows[i]);
        EXPECT_EQ(token.column, 1);
    }
}

TEST_F(ScannerInput, OperatorScan)
{
    UseFile("tests/scanner_test_files/operators.tl");
    token_t token;

    term_type_t terms[] = { T_LT,       T_GT,           T_LTE,          T_GTE,  T_DOUBLE_EQUALS,
                            T_EQUALS,   T_TILDE_EQUALS, T_DOUBLE_DOT,   T_PLUS, T_MINUS,
                            T_ASTERISK, T_SLASH,        T_DOUBLE_SLASH, T_HASH, T_EOF };

    for(int i = 0; i < 15; i++) {
        ASSERT_EQ(get_next_token(&token), E_OK);
        EXPECT_EQ(token.token_type, terms[i]);
        // i + 1, because rows are numbered from one
        EXPECT_EQ(token.row, i + 1);
        EXPECT_EQ(token.column, 1);
    }
}

TEST_F(ScannerInput, CommentScan)
{
    UseFile("tests/scanner_test_files/comments.tl");
    token_t token;
    int64_t ints[] = { 45, 56, 78 };
    int pos_rows[] = { 4, 5, 8 };
    int pos_cols[] = { 3, 50, 1 };

    for(int i = 0; i < 3; i++) {
        ASSERT_EQ(get_next_token(&token), E_OK);
        EXPECT_EQ(token.integer, ints[i]);
        EXPECT_EQ(token.row, pos_rows[i]);
        EXPECT_EQ(token.column, pos_cols[i]);
    }
}

TEST_F(ScannerInput, StringScan)
{
    UseFile("tests/scanner_test_files/strings.tl");
    token_t token;

    const char *s1 = "abc";
    const char *s2 = " def";
    const char *s3 = "\nghi\n";
    const char *s4 = "Everything you need to know for know";
    const char *s5 = "Lorem ipsum dolor sit amet, consect";

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s1), 0);
    EXPECT_EQ(token.row, 1);
    EXPECT_EQ(token.column, 1);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s2), 0);
    EXPECT_EQ(token.row, 1);
    EXPECT_EQ(token.column, 7);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s3), 0);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 1);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s4), 0);
    EXPECT_EQ(token.row, 6);
    EXPECT_EQ(token.column, 1);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s5), 0);
    EXPECT_EQ(token.row, 6);
    EXPECT_EQ(token.column, 39);
    str_free(&token.string);
}

TEST_F(ScannerInput, EscapeScan)
{
    UseFile("tests/scanner_test_files/esc_seq.tl");
    token_t token;

    const char *s1 = "Ahoj\n\"Sve'te \\\"";
    const char *s2 = "0123456789";

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s1), 0);
    EXPECT_EQ(token.row, 1);
    EXPECT_EQ(token.column, 1);

    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s2), 0);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 1);
    str_free(&token.string);
}

TEST_F(ScannerInput, KeywordScan)
{
    UseFile("tests/scanner_test_files/keywords.tl");
    token_t token;

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_DO);
    EXPECT_EQ(token.row, 1);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_ELSE);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_END);
    EXPECT_EQ(token.row, 3);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_FUNCTION);
    EXPECT_EQ(token.row, 4);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "_mem"), 0);
    EXPECT_EQ(token.row, 5);
    EXPECT_EQ(token.column, 1);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_GLOBAL);
    EXPECT_EQ(token.row, 6);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "globalVariable"), 0);
    EXPECT_EQ(token.row, 7);
    EXPECT_EQ(token.column, 1);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "_alsoGlobalVar156"), 0);
    EXPECT_EQ(token.row, 7);
    EXPECT_EQ(token.column, 16);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IF);
    EXPECT_EQ(token.row, 8);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "var_iable"), 0);
    EXPECT_EQ(token.row, 9);
    EXPECT_EQ(token.column, 1);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LOCAL);
    EXPECT_EQ(token.row, 10);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_NIL);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_REQUIRE);
    EXPECT_EQ(token.row, 12);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RETURN);
    EXPECT_EQ(token.row, 13);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_THEN);
    EXPECT_EQ(token.row, 14);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.row, 15);
    EXPECT_EQ(token.column, 1);
    EXPECT_EQ(token.token_type, T_WHILE);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.row, 16);
    EXPECT_EQ(token.column, 1);
    EXPECT_EQ(token.token_type, T_FOR);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "whileend"), 0);
    EXPECT_EQ(token.row, 17);
    EXPECT_EQ(token.column, 1);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EOF);
    EXPECT_EQ(token.row, 18);
    EXPECT_EQ(token.column, 1);
}

TEST_F(ScannerInput, ComplexProgram1)
{
    UseFile("tests/scanner_test_files/program1.tl");
    token_t token;

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_REQUIRE);
    EXPECT_EQ(token.row, 1);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "ifj21"), 0);
    EXPECT_EQ(token.row, 1);
    EXPECT_EQ(token.column, 9);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_FUNCTION);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "concat"), 0);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 10);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 17);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "x"), 0);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 18);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 20);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_STRING);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 22);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 28);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "y"), 0);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 30);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 32);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_STRING);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 34);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 40);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 42);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_STRING);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 44);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 50);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_INTEGER);
    EXPECT_EQ(token.row, 2);
    EXPECT_EQ(token.column, 52);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RETURN);
    EXPECT_EQ(token.row, 3);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "x"), 0);
    EXPECT_EQ(token.row, 3);
    EXPECT_EQ(token.column, 8);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_DOUBLE_DOT);
    EXPECT_EQ(token.row, 3);
    EXPECT_EQ(token.column, 10);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "y"), 0);
    EXPECT_EQ(token.row, 3);
    EXPECT_EQ(token.column, 13);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);
    EXPECT_EQ(token.row, 3);
    EXPECT_EQ(token.column, 14);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 0);
    EXPECT_EQ(token.token_type, T_INTEGER);
    EXPECT_EQ(token.row, 3);
    EXPECT_EQ(token.column, 16);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_END);
    EXPECT_EQ(token.row, 3);
    EXPECT_EQ(token.column, 18);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_FUNCTION);
    EXPECT_EQ(token.row, 4);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "main"), 0);
    EXPECT_EQ(token.row, 4);
    EXPECT_EQ(token.column, 10);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);
    EXPECT_EQ(token.row, 4);
    EXPECT_EQ(token.column, 14);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);
    EXPECT_EQ(token.row, 4);
    EXPECT_EQ(token.column, 15);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LOCAL);
    EXPECT_EQ(token.row, 5);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "x"), 0);
    EXPECT_EQ(token.row, 5);
    EXPECT_EQ(token.column, 7);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);
    EXPECT_EQ(token.row, 5);
    EXPECT_EQ(token.column, 9);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_STRING);
    EXPECT_EQ(token.row, 5);
    EXPECT_EQ(token.column, 11);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LOCAL);
    EXPECT_EQ(token.row, 6);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "ret"), 0);
    EXPECT_EQ(token.row, 6);
    EXPECT_EQ(token.column, 7);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);
    EXPECT_EQ(token.row, 6);
    EXPECT_EQ(token.column, 10);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_INTEGER);
    EXPECT_EQ(token.row, 6);
    EXPECT_EQ(token.column, 12);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "x"), 0);
    EXPECT_EQ(token.row, 7);
    EXPECT_EQ(token.column, 1);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);
    EXPECT_EQ(token.row, 7);
    EXPECT_EQ(token.column, 2);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "ret"), 0);
    EXPECT_EQ(token.row, 7);
    EXPECT_EQ(token.column, 4);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);
    EXPECT_EQ(token.row, 7);
    EXPECT_EQ(token.column, 8);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "concat"), 0);
    EXPECT_EQ(token.row, 7);
    EXPECT_EQ(token.column, 10);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);
    EXPECT_EQ(token.row, 7);
    EXPECT_EQ(token.column, 16);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "ahoj"), 0);
    EXPECT_EQ(token.row, 7);
    EXPECT_EQ(token.column, 17);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);
    EXPECT_EQ(token.row, 7);
    EXPECT_EQ(token.column, 23);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "svete"), 0);
    EXPECT_EQ(token.row, 7);
    EXPECT_EQ(token.column, 25);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);
    EXPECT_EQ(token.row, 7);
    EXPECT_EQ(token.column, 32);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_END);
    EXPECT_EQ(token.row, 8);
    EXPECT_EQ(token.column, 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "main"), 0);
    EXPECT_EQ(token.row, 9);
    EXPECT_EQ(token.column, 1);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);
    EXPECT_EQ(token.row, 9);
    EXPECT_EQ(token.column, 5);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);
    EXPECT_EQ(token.row, 9);
    EXPECT_EQ(token.column, 6);
}

TEST_F(ScannerInput, ComplexProgram2)
{
    UseFile("tests/scanner_test_files/program2.tl");
    token_t token;

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_REQUIRE);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "ifj21"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_FUNCTION);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "main"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LOCAL);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "a"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LOCAL);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "vysl"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 0);
    EXPECT_EQ(token.token_type, T_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "write"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "Zadejte cislo pro vypocet faktorialu\n"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "a"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "readi"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IF);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "a"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_DOUBLE_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_NIL);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_THEN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "write"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "a je nil\n"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RETURN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_ELSE);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_END);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IF);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "a"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LT);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 0);
    EXPECT_EQ(token.token_type, T_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_THEN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "write"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "Faktorial nelze spocitat\n"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_ELSE);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "vysl"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 1);
    EXPECT_EQ(token.token_type, T_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_WHILE);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "a"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_GT);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 0);
    EXPECT_EQ(token.token_type, T_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_DO);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "vysl"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "vysl"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_ASTERISK);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "a"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "a"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "a"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_MINUS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 1);
    EXPECT_EQ(token.token_type, T_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_END);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "write"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "Vysledek je: "), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "vysl"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "\n"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_END);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_END);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "main"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EOF);
}

TEST_F(ScannerInput, ComplexProgram3)
{
    UseFile("tests/scanner_test_files/program3.tl");
    token_t token;

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_REQUIRE);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "ifj21"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_FUNCTION);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "main"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LOCAL);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_STRING);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "Toto je nejaky text"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LOCAL);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s2"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_STRING);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_DOUBLE_DOT);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, ", ktery jeste trochu obohatime"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "write"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "a"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s2"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LOCAL);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1len"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_HASH);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LOCAL);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1len4"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1len"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1len"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1len"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_MINUS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 4);
    EXPECT_EQ(token.token_type, T_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "substr"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s2"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1len"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1len4"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1len"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1len"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_PLUS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 1);
    EXPECT_EQ(token.token_type, T_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "write"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "4 znaky od"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1len"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, ". znaku v \""), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s2"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "\":"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "\n"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "write"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "Zadejte serazenou posloupnost vsech malych pismen a-h, "),
              0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "write"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "pricemz se pismena nesmeji v posloupnosti opakovat: "), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "reads"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IF);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TILDE_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_NIL);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_THEN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_WHILE);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TILDE_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "abcdefgh"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_DO);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "write"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "\n"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "Spatne zadana posloupnost, zkuste znovu:"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "s1"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "reads"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_END);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_ELSE);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_END);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_END);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "main"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);
}

TEST_F(ScannerInput, UngetToken)
{
    UseFile("tests/scanner_test_files/numbers.tl");
    token_t token;

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 123);
    EXPECT_EQ(token.token_type, T_INTEGER);

    ASSERT_EQ(unget_token(), 0);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 123);
    EXPECT_EQ(token.token_type, T_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 3.14);
    EXPECT_EQ(token.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 3.1);
    EXPECT_EQ(token.token_type, T_NUMBER);

    ASSERT_EQ(unget_token(), 0);

    ASSERT_EQ(unget_token(), 0);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 3.14);
    EXPECT_EQ(token.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 3.1);
    EXPECT_EQ(token.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 3123.0000001);
    EXPECT_EQ(token.token_type, T_NUMBER);

    ASSERT_EQ(unget_token(), 0);

    ASSERT_EQ(unget_token(), 0);

    ASSERT_EQ(unget_token(), 1);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 3.1);
    EXPECT_EQ(token.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 3123.0000001);
    EXPECT_EQ(token.token_type, T_NUMBER);
}
