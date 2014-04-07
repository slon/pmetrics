#include <cmath>
#include <random>

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

    for(int i = 0; i < 100000; i++) {
        now += std::chrono::milliseconds(1);
        c.mark(now);
    }

    EXPECT_NEAR(c.value(now), 3000, 1.0);
}

TEST(linear_mapping_test_t, full) {
    linear_mapping_t mapping(10.0, 40.0, 10);

    ASSERT_EQ(10, mapping.n_buckets());

    ASSERT_EQ(10.0, mapping.unmap(0));
    ASSERT_EQ(37.0, mapping.unmap(9));
    ASSERT_EQ(40.0, mapping.unmap(10));

    ASSERT_EQ(0, mapping.map(0));
    ASSERT_EQ(0, mapping.map(10));

    ASSERT_EQ(9, mapping.map(40));
    ASSERT_EQ(9, mapping.map(50));

    ASSERT_EQ(4, mapping.map(23));
}

TEST(histogram_counter_test_t, decay) {
    histogram_counter_t histogram(std::chrono::seconds(1), linear_mapping_t(0, 100, 100));

    std::default_random_engine generator;
    std::poisson_distribution<int> poisson(/* mean = */ 10);

    auto now = std::chrono::system_clock::now();
    for(int i = 0; i < 1000000; ++i) {
        now += std::chrono::milliseconds(1);
        histogram.update(now, poisson(generator));
    }

    std::vector<double> q = { 0.5, 0.8, 0.9, 0.95, 0.99, 0.999 };
    std::vector<double> qvalues;

    histogram.get_quantiles(now, q, &qvalues);

    EXPECT_NEAR(10., qvalues[0], 2);
    EXPECT_NEAR(13., qvalues[1], 2);
    EXPECT_NEAR(14., qvalues[2], 2);
    EXPECT_NEAR(15., qvalues[3], 2);
    EXPECT_NEAR(18., qvalues[4], 2);
    EXPECT_NEAR(21., qvalues[5], 2);
}
