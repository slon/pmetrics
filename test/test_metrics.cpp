#include <pm/metrics.h>
#include <pm/graphite.h>

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

TEST(metrics_test_t, counter) {
    counter_t c = get_root().subtree("test").counter("counter");

    c.set(-2);
    c.inc(10);
    c.dec(3);

    graphite_printer_t p("g", 100);
    get_root().print(&p);

    ASSERT_EQ("g.test.counter 5 100\n", p.result());
}
