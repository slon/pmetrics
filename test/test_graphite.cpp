#include <pm/graphite.h>

#include <gtest/gtest.h>

using namespace pm;

TEST(graphite_printer_test_t, empty) {
    graphite_printer_t p("one_min.yandex");

    ASSERT_EQ("", p.result());
}

TEST(graphite_printer_test_t, simple) {
    graphite_printer_t p("one_min.yandex", /* timestamp = */ 15);

    p.start_node();

    p.child("foo");
    p.value((int64_t)10);

    p.child("bar");
    p.start_node();
    p.child("value.value");
    p.value(0.5);
    p.end_node();

    p.end_node();

    ASSERT_EQ(
        "one_min.yandex.foo 10 15\n"
        "one_min.yandex.bar.value_value 0.5 15\n",
        p.result());
}
