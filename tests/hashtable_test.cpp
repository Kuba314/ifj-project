#include <stdexcept>
#include <gtest/gtest.h>

extern "C" {
#include "hashtable_bst.h"
#include "error.h"
}

static uint64_t hash(const char *key)
{
    uint64_t h = 0;
    for(const char *c = key; *c != '\0'; ++c) {
        h = ((h << 5) ^ (h >> 27)) ^ *c;
    }
    return h;
}

class HashtableEmpty : public ::testing::Test {

  protected:
    hashtable_t map;

    virtual void SetUp() override
    {
        if(hashtable_create_bst(&map, 43, hash) != E_OK) {
            throw std::bad_alloc();
        }
    }

    virtual void TearDown() override
    {
        hashtable_free(&map);
    }

    void insert(const char *key, int *data)
    {
        EXPECT_EQ(hashtable_insert(&map, key, data), E_OK);
    };

    void find(const char *key, int value)
    {
        int *get;
        EXPECT_EQ(hashtable_find(&map, key, (void **) &get), E_OK);
        EXPECT_EQ(*get, value);
    };

    void findNull(const char *key)
    {
        int *get;
        EXPECT_EQ(hashtable_find(&map, key, (void **) &get), E_INT);
    }
};

TEST(Hashtable, Create)
{
    hashtable_t map;
    EXPECT_EQ(hashtable_create_bst(&map, 43, hash), E_OK);
    hashtable_free(&map);
}

TEST_F(HashtableEmpty, Insert)
{
    int d1 = 1, d2 = 2, d3 = 3, d4 = 4;
    insert("key1", &d1);
    insert("key2", &d2);
    insert("key3", &d3);
    insert("key4", &d4);

    find("key1", 1);
    find("key2", 2);
    find("key3", 3);
    find("key4", 4);

    int *get;
    EXPECT_EQ(hashtable_find(&map, "k5", (void **) &get), E_INT);
}

TEST_F(HashtableEmpty, IsEmpty)
{
    EXPECT_TRUE(hashtable_empty(&map));
    int d1 = 1;
    insert("key1", &d1);
    EXPECT_FALSE(hashtable_empty(&map));
    EXPECT_GT(hashtable_load_factor(&map), 0.0);
}

TEST_F(HashtableEmpty, Erase)
{
    int d1 = 1, d2 = 2, d3 = 3, d4 = 4;
    insert("key1", &d1);
    insert("key2", &d2);
    insert("key3", &d3);
    insert("key4", &d4);

    hashtable_erase(&map, "key2");
    find("key1", 1);
    findNull("key2");
    find("key3", 3);
    find("key4", 4);

    hashtable_erase(&map, "key1");
    findNull("key1");
    find("key3", 3);
    find("key4", 4);

    hashtable_erase(&map, "key3");
    findNull("key3");
    find("key4", 4);

    hashtable_erase(&map, "key4");
    findNull("key4");
}
