#include <stdexcept>
#include <gtest/gtest.h>

extern "C" {
#include "stack.h"
#include "error.h"
}

class EmptyStack : public ::testing::Test {

  protected:
    adt_stack_t stack;

    virtual void SetUp() override
    {
        // throw exception if allocation failed
        if(stack_create(&stack, 1) != E_OK) {
            throw std::bad_alloc();
        }
    }
    virtual void TearDown() override
    {
        stack_free(&stack);
    }
};

TEST(StackCreation, CreateEmpty)
{
    adt_stack_t stack;
    ASSERT_EQ(stack_create(&stack, 10), E_OK);
    stack_free(&stack);
}

TEST_F(EmptyStack, CheckEmpty)
{
    EXPECT_EQ(stack.size, 0);
    EXPECT_EQ(stack_top(&stack), nullptr);
    EXPECT_EQ(stack_empty(&stack), true);
}

TEST_F(EmptyStack, PushOneElement)
{
    int data = 10;
    EXPECT_EQ(stack_push(&stack, &data), E_OK);
    EXPECT_EQ(stack.size, 1);
}

TEST_F(EmptyStack, PushTwoElements)
{
    int data1 = 10, data2 = 20;
    stack_push(&stack, &data1);
    EXPECT_EQ(stack_push(&stack, &data2), E_OK);
    EXPECT_EQ(stack.size, 2);
}

TEST_F(EmptyStack, PopOneElement)
{
    int data = 10;
    stack_push(&stack, &data);
    EXPECT_FALSE(stack_empty(&stack));
    EXPECT_NE(stack_top(&stack), nullptr);
    EXPECT_NE(stack_pop(&stack), nullptr);
    EXPECT_TRUE(stack_empty(&stack));
    EXPECT_EQ(stack_top(&stack), nullptr);
}

TEST_F(EmptyStack, PopTwoElements)
{
    int data1 = 10, data2 = 20;
    stack_push(&stack, &data1);
    stack_push(&stack, &data2);
    EXPECT_FALSE(stack_empty(&stack));
    EXPECT_NE(stack_top(&stack), nullptr);
    EXPECT_NE(stack_pop(&stack), nullptr);
    EXPECT_FALSE(stack_empty(&stack));
    EXPECT_NE(stack_top(&stack), nullptr);
    EXPECT_NE(stack_pop(&stack), nullptr);
    EXPECT_TRUE(stack_empty(&stack));
    EXPECT_EQ(stack_top(&stack), nullptr);
}

TEST_F(EmptyStack, CheckPopFail)
{
    EXPECT_EQ(stack_pop(&stack), nullptr);
}

TEST_F(EmptyStack, CheckOneElement)
{
    int data = 10;
    stack_push(&stack, &data);
    ASSERT_NE(stack_top(&stack), nullptr);
    int get = *(int *) stack_top(&stack);
    ASSERT_EQ(get, 10);
}

TEST_F(EmptyStack, CheckPop)
{
    int data = 10;
    stack_push(&stack, &data);
    ASSERT_NE(stack_top(&stack), nullptr);
    int get1 = *(int *) stack_pop(&stack);
    EXPECT_EQ(get1, 10);
    EXPECT_TRUE(stack_empty(&stack));
    EXPECT_EQ(stack_top(&stack), nullptr);
}

TEST_F(EmptyStack, CheckTwoElements)
{
    int data1 = 10, data2 = 20;
    stack_push(&stack, &data1);
    stack_push(&stack, &data2);
    ASSERT_NE(stack_top(&stack), nullptr);
    int get2 = *(int *) stack_top(&stack);
    EXPECT_EQ(get2, 20);
    stack_pop(&stack);
    ASSERT_NE(stack_top(&stack), nullptr);
    int get1 = *(int *) stack_pop(&stack);
    EXPECT_EQ(get1, 10);
    EXPECT_TRUE(stack_empty(&stack));
    EXPECT_EQ(stack_top(&stack), nullptr);
}
