#include <stdexcept>
#include <string.h>

#include <gtest/gtest.h>
extern "C" {
#include "dynstring.h"
}

class EmptyString : public ::testing::Test {
  protected:
    virtual void SetUp() override
    {
        // throw exception if allocation failed
        if(str_create_empty(&str)) {
            throw std::bad_alloc();
        }
    }
    virtual void TearDown() override
    {
        str_free(&str);
    }

    string_t str;
};

TEST(StringCreation, CreateEmpty)
{
    string_t str;
    ASSERT_EQ(str_create_empty(&str), 0);
    ASSERT_NE(str.ptr, nullptr);
    str_free(&str);
}
TEST(StringCreation, CreateFromString)
{
    const char *sample_string = "Once upon a time...";

    string_t str;
    ASSERT_EQ(str_create(sample_string, &str), 0);
    EXPECT_EQ(strcmp(sample_string, str.ptr), 0);
    str_free(&str);
}
TEST(StringCreation, CreateFromHugeString)
{
    const char *sample_string =
        "Lorem ipsum dolor sit amet consectetur adipisicing elit. Maxime "
        "mollitia, molestiae quas vel sint commodi repudiandae consequuntur "
        "voluptatum laborum numquam blanditiis harum quisquam eius sed odit "
        "fugiat iusto fuga praesentium optio, eaque rerum !";

    string_t str;
    ASSERT_EQ(str_create(sample_string, &str), 0);
    EXPECT_EQ(strcmp(sample_string, str.ptr), 0);
    str_free(&str);
}

TEST_F(EmptyString, CheckNullByte)
{
    EXPECT_EQ(str.ptr[0], '\0');
}
TEST_F(EmptyString, CheckZeroLength)
{
    EXPECT_EQ(str.length, 0);
}
TEST_F(EmptyString, AppendChar)
{
    ASSERT_EQ(str_append_char(&str, 'a'), 0);
    EXPECT_EQ(str.ptr[0], 'a');
    EXPECT_EQ(str.ptr[1], '\0');
    EXPECT_EQ(str.length, 1);
}
TEST_F(EmptyString, AppendCharMultiple)
{
    const char *sample_string = "in a tiny lived a tiny chick...";
    size_t str_size = strlen(sample_string);

    for(size_t i = 0; i < str_size; i++) {
        ASSERT_EQ(str_append_char(&str, sample_string[i]), 0);
    }
    EXPECT_EQ(str.length, str_size);
    EXPECT_EQ(str.ptr[str_size], '\0');
}
