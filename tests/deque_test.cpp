#include <stdexcept>
#include <gtest/gtest.h>

extern "C" {
#include "deque.h"
#include "error.h"
}

class EmptyDeque : public ::testing::Test {

  protected:
    deque_t deque;

    virtual void SetUp() override
    {
        deque_create(&deque);
    }

    virtual void TearDown() override
    {
        deque_free(&deque);
    }

    void testElements(const std::vector<int> &data)
    {
        deque_element_t *e = deque_front_element(&deque);
        for(size_t i = 0; i < data.size(); ++i) {
            ASSERT_NE(e, nullptr);
            EXPECT_EQ(*(int *) e->data, data[i]);
            if(i == 0) {
                EXPECT_EQ(e->prev, nullptr);
            }
            if(i == data.size() - 1) {
                EXPECT_EQ(e->next, nullptr);
            }
            e = e->next;
        }
    }
};

TEST_F(EmptyDeque, CheckEmpty)
{
    EXPECT_EQ(deque.size, 0);
    EXPECT_EQ(deque_front(&deque), nullptr);
    EXPECT_EQ(deque_back(&deque), nullptr);
    EXPECT_EQ(deque_empty(&deque), true);
    EXPECT_EQ(deque_pop_front(&deque), nullptr);
    EXPECT_EQ(deque_pop_back(&deque), nullptr);
    EXPECT_NO_THROW(deque_erase(&deque, nullptr));
}

TEST_F(EmptyDeque, PushOneElement)
{
    int data = 10;
    EXPECT_EQ(deque_push_front(&deque, &data), E_OK);
    EXPECT_EQ(deque.size, 1);
    testElements({ data });
}

TEST_F(EmptyDeque, PushTwoElementsMethodA)
{
    int data1 = 10, data2 = 20;
    EXPECT_EQ(deque_push_front(&deque, &data2), E_OK);
    EXPECT_EQ(deque_push_front(&deque, &data1), E_OK);
    EXPECT_EQ(deque.size, 2);
    testElements({ data1, data2 });
}

TEST_F(EmptyDeque, PushTwoElementsMethodB)
{
    int data1 = 10, data2 = 20;
    EXPECT_EQ(deque_push_front(&deque, &data1), E_OK);
    EXPECT_EQ(deque_push_back(&deque, &data2), E_OK);
    EXPECT_EQ(deque.size, 2);
    testElements({ data1, data2 });
}

TEST_F(EmptyDeque, PushTwoElementsMethodC)
{
    int data1 = 10, data2 = 20;
    EXPECT_EQ(deque_push_back(&deque, &data2), E_OK);
    EXPECT_EQ(deque_push_front(&deque, &data1), E_OK);
    EXPECT_EQ(deque.size, 2);
    testElements({ data1, data2 });
}

TEST_F(EmptyDeque, PushTwoElementsMethodD)
{
    int data1 = 10, data2 = 20;
    EXPECT_EQ(deque_push_back(&deque, &data1), E_OK);
    EXPECT_EQ(deque_push_back(&deque, &data2), E_OK);
    EXPECT_EQ(deque.size, 2);
    testElements({ data1, data2 });
}

void t(deque_t *deque)
{
    deque_element_t *e = deque_front_element(deque);
    printf("{\n");
    while(e) {
        printf("  %d\n", *(int *) e->data);
        e = e->next;
    }
    printf("}\n");
}

TEST_F(EmptyDeque, PopElementsMethodA)
{
    int data1 = 10, data2 = 20;
    EXPECT_EQ(deque_push_back(&deque, &data1), E_OK);
    EXPECT_EQ(deque_push_back(&deque, &data2), E_OK);
    EXPECT_NE(deque_pop_front(&deque), nullptr);
    testElements({ data2 });
    EXPECT_NE(deque_pop_back(&deque), nullptr);
    EXPECT_TRUE(deque_empty(&deque));
}

TEST_F(EmptyDeque, PopElementsMethodB)
{
    int data1 = 10, data2 = 20;
    EXPECT_EQ(deque_push_back(&deque, &data1), E_OK);
    EXPECT_EQ(deque_push_back(&deque, &data2), E_OK);
    EXPECT_NE(deque_pop_back(&deque), nullptr);
    testElements({ data1 });
    EXPECT_NE(deque_pop_front(&deque), nullptr);
    EXPECT_TRUE(deque_empty(&deque));
}

TEST_F(EmptyDeque, Insert)
{
    int data1 = 10, data2 = 20, data3 = 30;
    EXPECT_EQ(deque_insert(&deque, deque_front_element(&deque), &data3), E_OK);
    EXPECT_EQ(deque_insert(&deque, deque_front_element(&deque), &data1), E_OK);
    EXPECT_EQ(deque_insert(&deque, deque_back_element(&deque), &data2), E_OK);
    testElements({ data1, data2, data3 });
}

TEST_F(EmptyDeque, Erase)
{
    int data1 = 10, data2 = 20, data3 = 30;
    EXPECT_EQ(deque_push_back(&deque, &data1), E_OK);
    EXPECT_EQ(deque_push_back(&deque, &data2), E_OK);
    EXPECT_EQ(deque_push_back(&deque, &data3), E_OK);
    testElements({ data1, data2, data3 });

    deque_element_t *e = deque_front_element(&deque);
    ASSERT_NE(e, nullptr);
    deque_erase(&deque, e->next);
    testElements({ data1, data3 });

    deque_erase(&deque, deque_front_element(&deque));
    testElements({ data3 });

    deque_erase(&deque, deque_back_element(&deque));
    EXPECT_TRUE(deque_empty(&deque));
}
