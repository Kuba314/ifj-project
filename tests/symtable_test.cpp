
#include <stdexcept>
#include <gtest/gtest.h>

extern "C" {
#include "symtable.h"
#include "error.h"
}

struct test_data_t {
    string_t name;

    test_data_t(std::string str)
    {
        if(str_create_empty(&name) != E_OK) {
            throw std::bad_alloc();
        }
        for(char c : str) {
            if(str_append_char(&name, c) != E_OK) {
                throw std::bad_alloc();
            }
        }
    }

    ~test_data_t()
    {
        str_free(&name);
    }
};

class EmptySymtable : public ::testing::Test {

  protected:
    virtual void SetUp() override
    {
        if(symtable_init() != E_OK) {
            throw std::bad_alloc();
        }
    }
    virtual void TearDown() override
    {
        symtable_free();
    }
};

TEST_F(EmptySymtable, CheckEmpty)
{
    EXPECT_EQ(symtable_find("x"), nullptr);
}

TEST_F(EmptySymtable, AddScope)
{
    EXPECT_EQ(symtable_push_scope(), E_OK); // push local scope
    EXPECT_EQ(symtable_pop_scope(), E_OK);  // push local scope
    EXPECT_EQ(symtable_pop_scope(), E_OK);  // pop global scope
    EXPECT_EQ(symtable_pop_scope(), E_INT); // invalid
}

TEST_F(EmptySymtable, CheckSymbol)
{
    test_data_t data("x");
    EXPECT_EQ(symtable_put_symbol("x", (symbol_t *) &data), E_OK);
    test_data_t *ref = (test_data_t *) symtable_find("x");
    ASSERT_NE(ref, nullptr);
    EXPECT_EQ(ref->name.ptr, data.name.ptr);
}

TEST_F(EmptySymtable, CheckTwoSymbols)
{
    test_data_t data1("x");
    test_data_t data2("y");
    EXPECT_EQ(symtable_put_symbol("x", (symbol_t *) &data1), E_OK);
    EXPECT_EQ(symtable_put_symbol("y", (symbol_t *) &data2), E_OK);

    test_data_t *ref1 = (test_data_t *) symtable_find("x");
    ASSERT_NE(ref1, nullptr);
    test_data_t *ref2 = (test_data_t *) symtable_find("y");
    ASSERT_NE(ref2, nullptr);
    EXPECT_EQ(ref1->name.ptr, data1.name.ptr);
    EXPECT_EQ(ref2->name.ptr, data2.name.ptr);
}

TEST_F(EmptySymtable, CheckTwoSymbolsMultipleScopes)
{
    test_data_t data1("x");
    test_data_t data2("y");
    EXPECT_EQ(symtable_put_symbol("x", (symbol_t *) &data1), E_OK);

    EXPECT_EQ(symtable_push_scope(), E_OK);

    EXPECT_EQ(symtable_put_symbol("y", (symbol_t *) &data2), E_OK);

    test_data_t *ref1 = (test_data_t *) symtable_find("x");
    ASSERT_NE(ref1, nullptr);
    test_data_t *ref2 = (test_data_t *) symtable_find("y");
    ASSERT_NE(ref2, nullptr);
    EXPECT_EQ(ref1->name.ptr, data1.name.ptr);
    EXPECT_EQ(ref2->name.ptr, data2.name.ptr);
}

TEST_F(EmptySymtable, CheckSymbolOvershadowing)
{
    test_data_t data1("x");
    test_data_t data2("y");
    EXPECT_EQ(symtable_put_symbol("x", (symbol_t *) &data1), E_OK);

    test_data_t *ref = (test_data_t *) symtable_find("x");
    ASSERT_NE(ref, nullptr);
    EXPECT_EQ(ref->name.ptr, data1.name.ptr);

    EXPECT_EQ(symtable_push_scope(), E_OK);

    ref = (test_data_t *) symtable_find("x");
    ASSERT_NE(ref, nullptr);
    EXPECT_EQ(ref->name.ptr, data1.name.ptr);

    EXPECT_EQ(symtable_put_symbol("x", (symbol_t *) &data2), E_OK);

    ref = (test_data_t *) symtable_find("x");
    ASSERT_NE(ref, nullptr);
    EXPECT_EQ(ref->name.ptr, data2.name.ptr);
}

TEST_F(EmptySymtable, CheckFindCurrent)
{
    test_data_t data1("x");
    EXPECT_EQ(symtable_put_symbol("x", (symbol_t *) &data1), E_OK);

    EXPECT_EQ(symtable_push_scope(), E_OK);

    test_data_t data2("y");
    EXPECT_EQ(symtable_put_symbol("y", (symbol_t *) &data2), E_OK);

    test_data_t *ref = (test_data_t *) symtable_find_in_current("x");
    EXPECT_EQ(ref, nullptr);

    ref = (test_data_t *) symtable_find_in_current("y");
    ASSERT_NE(ref, nullptr);
    EXPECT_EQ(ref->name.ptr, data2.name.ptr);
}

TEST_F(EmptySymtable, CheckFindGlobal)
{
    test_data_t data1("x#0");
    EXPECT_EQ(symtable_put_in_global("x", (symbol_t *) &data1), E_OK);

    EXPECT_EQ(symtable_push_scope(), E_OK);

    test_data_t data2("x#1");
    EXPECT_EQ(symtable_put_symbol("x", (symbol_t *) &data2), E_OK);

    test_data_t *ref = (test_data_t *) symtable_find("x");
    ASSERT_NE(ref, nullptr);
    EXPECT_EQ(ref->name.ptr, data2.name.ptr);

    ref = (test_data_t *) symtable_find_in_global("x");
    ASSERT_NE(ref, nullptr);
    EXPECT_EQ(ref->name.ptr, data1.name.ptr);
}

TEST_F(EmptySymtable, CheckMangling)
{
    test_data_t data1("x");
    EXPECT_EQ(symtable_create_mangled_id(&data1.name), E_OK);
    EXPECT_EQ(symtable_put_symbol("x", (symbol_t *) &data1), E_OK);

    test_data_t *ref = (test_data_t *) symtable_find("x");
    ASSERT_NE(ref, nullptr);
    EXPECT_EQ(std::string(ref->name.ptr), std::string("x#0"));

    EXPECT_EQ(symtable_push_scope(), E_OK);

    test_data_t data2("x");
    EXPECT_EQ(symtable_create_mangled_id(&data2.name), E_OK);
    EXPECT_EQ(symtable_put_symbol("x", (symbol_t *) &data2), E_OK);

    ref = (test_data_t *) symtable_find("x");
    ASSERT_NE(ref, nullptr);
    EXPECT_EQ(std::string(ref->name.ptr), std::string("x#1"));

    ref = (test_data_t *) symtable_find_in_global("x");
    ASSERT_NE(ref, nullptr);
    EXPECT_EQ(std::string(ref->name.ptr), std::string("x#0"));
}
