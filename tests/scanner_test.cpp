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

class ScannerInputNumbers : public ::testing::Test {
  protected:
    virtual void SetUp() override
    {
        f = fopen("tests/scanner_test_files/numbers.tl", "r");
        initialise_file_ptr(f);
    }
    virtual void TearDown() override
    {
        close_file(f);
    }
    FILE *f;
};

class ScannerInputOperators : public ::testing::Test {
  protected:
    virtual void SetUp() override
    {
        f = fopen("tests/scanner_test_files/operators.tl", "r");
        initialise_file_ptr(f);
    }
    virtual void TearDown() override
    {
        close_file(f);
    }
    FILE *f;
};

class ScannerInputComments : public ::testing::Test {
  protected:
    virtual void SetUp() override
    {
        f = fopen("tests/scanner_test_files/comments.tl", "r");
        initialise_file_ptr(f);
    }
    virtual void TearDown() override
    {
        close_file(f);
    }
    FILE *f;
};

class ScannerInputStrings : public ::testing::Test {
  protected:
    virtual void SetUp() override
    {
        f = fopen("tests/scanner_test_files/strings.tl", "r");
        initialise_file_ptr(f);
    }
    virtual void TearDown() override
    {
        close_file(f);
    }
    FILE *f;
};

class ScannerInputEscapeSeq : public ::testing::Test {
  protected:
    virtual void SetUp() override
    {
        f = fopen("tests/scanner_test_files/esc_seq.tl", "r");
        initialise_file_ptr(f);
    }
    virtual void TearDown() override
    {
        close_file(f);
    }
    FILE *f;
};

class ScannerInputKeywords : public ::testing::Test {
  protected:
    virtual void SetUp() override
    {
        f = fopen("tests/scanner_test_files/keywords.tl", "r");
        initialise_file_ptr(f);
    }
    virtual void TearDown() override
    {
        close_file(f);
    }
    FILE *f;
};

class ScannerInputProgram1 : public ::testing::Test {
  protected:
    virtual void SetUp() override
    {
        f = fopen("tests/scanner_test_files/program1.tl", "r");
        initialise_file_ptr(f);
    }
    virtual void TearDown() override
    {
        close_file(f);
    }
    FILE *f;
};

class ScannerInputProgram2 : public ::testing::Test {
  protected:
    virtual void SetUp() override
    {
        f = fopen("tests/scanner_test_files/program2.tl", "r");
        initialise_file_ptr(f);
    }
    virtual void TearDown() override
    {
        close_file(f);
    }
    FILE *f;
};

class ScannerInputProgram3 : public ::testing::Test {
  protected:
    virtual void SetUp() override
    {
        f = fopen("tests/scanner_test_files/program3.tl", "r");
        initialise_file_ptr(f);
    }
    virtual void TearDown() override
    {
        close_file(f);
    }
    FILE *f;
};

TEST_F(ScannerInputNumbers, NumberScan)
{
    token_t token;

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 123);
    EXPECT_EQ(token.token_type, T_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 3.14);
    EXPECT_EQ(token.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 3.1);
    EXPECT_EQ(token.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 3123.0000001);
    EXPECT_EQ(token.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 30000.0);
    EXPECT_EQ(token.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 3000.0);
    EXPECT_EQ(token.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 42.4e3);
    EXPECT_EQ(token.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 42.4e+3);
    EXPECT_EQ(token.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.number, (double) 42.4e-3);
    EXPECT_EQ(token.token_type, T_NUMBER);
}

TEST_F(ScannerInputOperators, OperatorScan)
{
    token_t token;

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LT);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_GT);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LTE);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_GTE);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_DOUBLE_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TILDE_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_DOUBLE_DOT);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_PLUS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_MINUS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_ASTERISK);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_SLASH);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_DOUBLE_SLASH);
}

TEST_F(ScannerInputComments, OperatorScan)
{
    token_t token;

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 45);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 56);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 78);
}

TEST_F(ScannerInputStrings, StringScan)
{
    token_t token;

    const char *s1 = "abc";
    const char *s2 = " def";
    const char *s3 = "\nghi\n";
    const char *s4 = "Everything you need to know for know";
    const char *s5 = "Lorem ipsum dolor sit amet, consect";

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s1), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s2), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s3), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s4), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s5), 0);
    str_free(&token.string);
}

TEST_F(ScannerInputEscapeSeq, EscapeScan)
{
    token_t token;

    const char *s1 = "Ahoj\n\"Sve'te \\\"";
    const char *s2 = "0123456789";

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s1), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(strcmp(token.string.ptr, s2), 0);
    str_free(&token.string);
}

TEST_F(ScannerInputKeywords, KeywordScan)
{
    token_t token;

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_DO);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_ELSE);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_END);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_FUNCTION);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "_mem"), 0);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_GLOBAL);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "globalVariable"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "_alsoGlobalVar156"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IF);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LOCAL);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_NIL);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_REQUIRE);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RETURN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_THEN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_WHILE);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "whileend"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EOF);
}

TEST_F(ScannerInputProgram1, ComplexProgram1)
{
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
    EXPECT_EQ(strcmp(token.string.ptr, "concat"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "x"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_STRING);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "y"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_STRING);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_STRING);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RETURN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "x"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_DOUBLE_DOT);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "y"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.integer, (int64_t) 0);
    EXPECT_EQ(token.token_type, T_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_END);

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
    EXPECT_EQ(strcmp(token.string.ptr, "x"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_STRING);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LOCAL);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "ret"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COLON);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_INTEGER);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "x"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "ret"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_EQUALS);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_IDENTIFIER);
    EXPECT_EQ(strcmp(token.string.ptr, "concat"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_LPAREN);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "ahoj"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_COMMA);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_STRING);
    EXPECT_EQ(strcmp(token.string.ptr, "svete"), 0);
    str_free(&token.string);

    ASSERT_EQ(get_next_token(&token), E_OK);
    EXPECT_EQ(token.token_type, T_RPAREN);

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

TEST_F(ScannerInputProgram2, ComplexProgram2)
{
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

TEST_F(ScannerInputProgram3, ComplexProgram3)
{
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
    EXPECT_EQ(token.token_type, T_TYPE);
    EXPECT_EQ(token.type, TYPE_NIL);

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

TEST_F(ScannerInputNumbers, UngetToken)
{
    token_t token1;
    token_t token2;
    token_t token3;

    ASSERT_EQ(get_next_token(&token1), E_OK);
    EXPECT_EQ(token1.integer, (int64_t) 123);
    EXPECT_EQ(token1.token_type, T_INTEGER);

    ASSERT_EQ(unget_token(&token1), 0);

    ASSERT_EQ(get_next_token(&token1), E_OK);
    EXPECT_EQ(token1.integer, (int64_t) 123);
    EXPECT_EQ(token1.token_type, T_INTEGER);

    ASSERT_EQ(get_next_token(&token1), E_OK);
    EXPECT_EQ(token1.number, (double) 3.14);
    EXPECT_EQ(token1.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token2), E_OK);
    EXPECT_EQ(token2.number, (double) 3.1);
    EXPECT_EQ(token2.token_type, T_NUMBER);

    ASSERT_EQ(unget_token(&token2), 0);

    ASSERT_EQ(unget_token(&token1), 0);

    ASSERT_EQ(get_next_token(&token1), E_OK);
    EXPECT_EQ(token1.number, (double) 3.14);
    EXPECT_EQ(token1.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token2), E_OK);
    EXPECT_EQ(token2.number, (double) 3.1);
    EXPECT_EQ(token2.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token3), E_OK);
    EXPECT_EQ(token3.number, (double) 3123.0000001);
    EXPECT_EQ(token3.token_type, T_NUMBER);

    ASSERT_EQ(unget_token(&token2), 0);

    ASSERT_EQ(unget_token(&token1), 0);

    ASSERT_EQ(unget_token(&token3), 1);

    ASSERT_EQ(get_next_token(&token1), E_OK);
    EXPECT_EQ(token1.number, (double) 3.14);
    EXPECT_EQ(token1.token_type, T_NUMBER);

    ASSERT_EQ(get_next_token(&token2), E_OK);
    EXPECT_EQ(token2.number, (double) 3.1);
    EXPECT_EQ(token2.token_type, T_NUMBER);
}
