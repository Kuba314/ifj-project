#include <stdexcept>
#include <gtest/gtest.h>
#include <memory>

extern "C" {
#include "binary_search_tree.h"
#include "error.h"
}

static void deleteNodeData(bst_node *node)
{
    if(!node) {
        return;
    }
    if(node->left) {
        deleteNodeData(node->left);
    }
    if(node->right) {
        deleteNodeData(node->right);
    }
    delete(int *) node->data;
}

using OrderedData = std::vector<void *>;

static OrderedData getInorder(bst_node_t *tree)
{
    OrderedData d;
    std::function<void(bst_node_t *)> inorder = [&inorder, &d](bst_node_t *node) -> void {
        if(!node) {
            return;
        }
        inorder(node->left);
        d.push_back(node->data);
        inorder(node->right);
    };
    inorder(tree);
    return d;
}

static bool compareData(const OrderedData &data, const std::vector<int> &reference)
{
    if(data.size() != reference.size()) {
        return false;
    }
    for(size_t i = 0; i < data.size(); ++i) {
        if(*(int *) data.at(i) != reference.at(i)) {
            return false;
        }
    }
    return true;
}

static std::string dataToString(const OrderedData &data)
{
    std::string s = "{";
    for(void *d : data) {
        s += std::to_string(*(int *) d);
        s += ", ";
    }
    s += "}";
    return s;
}

static std::string dataToString(const std::vector<int> &data)
{
    std::string s = "{";
    for(int d : data) {
        s += std::to_string(d);
        s += ", ";
    }
    s += "}";
    return s;
}

class BSTTwoFilled : public ::testing::Test {
  protected:
    virtual void SetUp() override
    {
        tree1 = NULL;
        tree2 = NULL;
        const auto insert = [&](bst_node_t **tree, const char *key, int value) {
            int *v = new int;
            *v = value;
            if(bst_insert(tree, key, v) != E_OK) {
                TearDown();
                throw std::bad_alloc();
            }
        };
        insert(&tree1, "k1", 1);
        insert(&tree1, "k2", 2);
        insert(&tree1, "k3", 3);
        insert(&tree1, "k4", 4);
        insert(&tree1, "k5", 5);
        insert(&tree1, "k6", 6);

        insert(&tree2, "k3", 3);
        insert(&tree2, "k5", 5);
        insert(&tree2, "k6", 6);
        insert(&tree2, "k2", 2);
        insert(&tree2, "k4", 4);
        insert(&tree2, "k1", 1);
    }

    virtual void TearDown() override
    {
        deleteNodeData(tree1);
        deleteNodeData(tree2);
        bst_free(tree1);
        bst_free(tree2);
    }

    void testBoth(const std::vector<int> &reference)
    {
        OrderedData data1 = getInorder(tree1);
        OrderedData data2 = getInorder(tree2);

        EXPECT_EQ(compareData(data1, reference), true)
            << "Failed on " << dataToString(data1) << " == " << dataToString(reference);
        EXPECT_EQ(compareData(data2, reference), true)
            << "Failed on " << dataToString(data2) << " == " << dataToString(reference);
    }

    void eraseNodeBoth(const char *key)
    {
        int *get;
        EXPECT_EQ(bst_find(tree1, key, (void **) &get), E_OK);
        delete get;
        EXPECT_EQ(bst_find(tree2, key, (void **) &get), E_OK);
        delete get;
        bst_erase(&tree1, key);
        bst_erase(&tree2, key);
    }

    bst_node_t *tree1;
    bst_node_t *tree2;
};

TEST(BST, Insert)
{
    // search_key_free(NULL);
    bst_node_t *tree = NULL;
    int d = 1;
    EXPECT_EQ(bst_insert(&tree, "k1", &d), E_OK);

    OrderedData data = getInorder(tree);
    EXPECT_TRUE(compareData(data, { 1 }));
    bst_free(tree);
}

TEST(BST, Erase)
{
    bst_node_t *tree = NULL;
    int d = 1;
    EXPECT_EQ(bst_insert(&tree, "k1", &d), E_OK);
    bst_erase(&tree, "k1");

    OrderedData data = getInorder(tree);
    EXPECT_TRUE(compareData(data, {}));
    bst_free(tree);
}

TEST(BST, Find)
{
    bst_node_t *tree = NULL;
    static const auto insert = [&](const char *key, int *data) {
        EXPECT_EQ(bst_insert(&tree, key, data), E_OK);
    };
    static const auto find = [&](const char *key, int value) {
        int *get;
        EXPECT_EQ(bst_find(tree, key, (void **) &get), E_OK);
        EXPECT_EQ(*get, value);
    };

    int d1 = 1, d2 = 2, d3 = 3;
    insert("k1", &d1);
    insert("k2", &d2);
    insert("k3", &d3);

    find("k1", 1);
    find("k2", 2);
    find("k3", 3);

    int *get;
    EXPECT_EQ(bst_find(tree, "k4", (void **) &get), E_INT);

    bst_free(tree);
}

TEST_F(BSTTwoFilled, Order)
{
    testBoth({ 1, 2, 3, 4, 5, 6 });
}

TEST_F(BSTTwoFilled, OrderWithDelete)
{
    eraseNodeBoth("k3");
    testBoth({ 1, 2, 4, 5, 6 });

    eraseNodeBoth("k6");
    testBoth({ 1, 2, 4, 5 });

    eraseNodeBoth("k4");
    testBoth({ 1, 2, 5 });

    eraseNodeBoth("k2");
    testBoth({ 1, 5 });

    eraseNodeBoth("k1");
    testBoth({ 5 });

    eraseNodeBoth("k5");
    testBoth({});
}
