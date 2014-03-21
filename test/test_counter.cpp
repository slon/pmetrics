#include <cmath>
#include <pm/counter.h>

#include <gtest/gtest.h>

using namespace pm;

TEST(double_buffer_counter_test_t, full) {
    auto now = std::chrono::system_clock::now();
    auto window_size = std::chrono::seconds(10);

    double_buffer_counter_t c(window_size);

    EXPECT_EQ(0, c.value(now));

    c.mark(now);
    c.mark(now + window_size / 2);
    c.mark(now + window_size / 2);

    EXPECT_EQ(0, c.value(now));
    EXPECT_EQ(0, c.value(now + window_size / 2));

    EXPECT_EQ(3, c.value(now + 3 * window_size / 2));

    c.mark(now + 3 * window_size / 2);

    EXPECT_EQ(3, c.value(now + 3 * window_size / 2));
    EXPECT_EQ(1, c.value(now + 3 * window_size));
}

TEST(exponential_decay_counter_test_t, simple) {
    auto now = std::chrono::system_clock::now();

    auto interval = std::chrono::seconds(1);
    decaying_counter_t c(interval);

    c.mark(now);
    EXPECT_EQ(c.value(now), 1);
}

TEST(exponential_decay_counter_test_t, decay) {
    auto now = std::chrono::system_clock::now();

    auto interval = std::chrono::seconds(3);
    decaying_counter_t c(interval);

    c.mark(now);
    c.mark(now + std::chrono::seconds(1));
    c.mark(now + std::chrono::seconds(2));
    c.mark(now + std::chrono::seconds(3));
    c.mark(now + std::chrono::seconds(4));
    c.mark(now + std::chrono::seconds(5));
    c.mark(now + std::chrono::seconds(6));
    c.mark(now + std::chrono::seconds(7));
    c.mark(now + std::chrono::seconds(8));

    EXPECT_NEAR(c.value(now + std::chrono::seconds(8)), 3, 0.6);
}
