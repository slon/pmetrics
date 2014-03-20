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
