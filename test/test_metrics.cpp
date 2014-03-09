#include <pm/metrics.h>

#include <gtest/gtest.h>

using namespace pm;

TEST(metrics_test_t, default_constructed_metrics_do_nothing) {
	counter_t counter;
	counter.inc();
	counter.dec();
	counter.set(0);

	meter_t meter;
	meter.mark();

	histogram_t hist;
	hist.update(10);

	pm::timer_t timer;
	timer.finish(timer.start());
	auto t = timer.time();	
}
