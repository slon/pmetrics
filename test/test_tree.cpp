#include <gtest/gtest.h>

#include <pm/tree.h>
#include <pm/graphite.h>

using namespace pm;

struct test_tree_leaf_t : public tree_leaf_t {
    test_tree_leaf_t(int64_t v) : value(v) {}

    virtual void print(tree_printer_t* printer) { printer->value(value); }

    int64_t value;
};

struct tree_test_t : public testing::Test {
    tree_test_t() : graphite("com.example", 15) {}

    tree_branch_t root;
    graphite_printer_t graphite;

    std::shared_ptr<test_tree_leaf_t> make_leaf(int v) {
        return std::make_shared<test_tree_leaf_t>(v);
    }
};

TEST_F(tree_test_t, empty) {
    root.print(&graphite);
    ASSERT_EQ("", graphite.result());
}

TEST_F(tree_test_t, one_node) {
    auto leaf = std::make_shared<test_tree_leaf_t>(10);
    root.add_leaf("foo", leaf);

    root.print(&graphite);
    ASSERT_EQ("com.example.foo 10 15\n", graphite.result());
}

TEST_F(tree_test_t, many_nodes) {
    auto leaf1 = make_leaf(1);
    auto leaf2 = make_leaf(2);
    auto leaf3 = make_leaf(3);

    root.add_leaf("leaf1", leaf1);
    root.get_branch("bar")->add_leaf("leaf2", leaf2);
    root.get_branch("bar")->add_leaf("leaf3", leaf3);

    root.print(&graphite);
    ASSERT_EQ(
        "com.example.bar.leaf2 2 15\n"
        "com.example.bar.leaf3 3 15\n"
        "com.example.leaf1 1 15\n",
        graphite.result());
}

TEST_F(tree_test_t, do_not_print_dead_leafs) {
    root.add_leaf("dead_leaf1", make_leaf(1));

    root.print(&graphite);
    ASSERT_EQ("", graphite.result());
}

TEST_F(tree_test_t, remove_empty_nodes) {
    auto leaf1 = make_leaf(1);
    auto leaf2 = make_leaf(2);

    root.add_leaf("live_leaf1", leaf1);
    root.add_leaf("dead_leaf4", make_leaf(4));

    std::weak_ptr<tree_branch_t> dead_branch =
        root.get_branch("dead_branch")->get_branch("sub");
    dead_branch.lock()->add_leaf("dead_leaf5", make_leaf(5));

    std::shared_ptr<tree_branch_t> live_branch1 =
        root.get_branch("live_branch1");
    std::weak_ptr<tree_branch_t> live_branch2 =
        root.get_branch("live_branch2")->get_branch("sub");
    live_branch2.lock()->add_leaf("live_leaf2", leaf2);

    root.remove_empty_nodes();

    ASSERT_TRUE(dead_branch.expired());
    ASSERT_FALSE(live_branch2.expired());
    ASSERT_EQ(2, live_branch1.use_count());
}
