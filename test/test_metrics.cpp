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

TEST(metrics_test_t, meter) {
    meter_t m = get_root().subtree("test").meter("meter");

    m.mark();

    graphite_printer_t p("g", 100);
    get_root().print(&p);

    ASSERT_EQ(
        "g.test.meter.one_sec 0 100\n"
        "g.test.meter.one_min 1 100\n"
        "g.test.meter.quarter_hour 1 100\n"
        "g.test.meter.one_hour 1 100\n",
        p.result());
}

TEST(metrics_test_t, timer) {
    pm::timer_t t = get_root().subtree("test").timer("timer");

    auto token = t.time();

    graphite_printer_t p("g", 100);
    get_root().print(&p);

    ASSERT_EQ(
        "g.test.timer.active 1 100\n"
        "g.test.timer.rate.one_sec 0 100\n"
        "g.test.timer.rate.one_min 1 100\n"
        "g.test.timer.rate.quarter_hour 1 100\n"
        "g.test.timer.rate.one_hour 1 100\n"
        "g.test.timer.timings.q50 0 100\n"
        "g.test.timer.timings.q80 0 100\n"
        "g.test.timer.timings.q90 0 100\n"
        "g.test.timer.timings.q95 0 100\n"
        "g.test.timer.timings.q99 0 100\n",
        p.result());

    token.finish();

    graphite_printer_t p2("g", 100);
    get_root().print(&p2);

    ASSERT_EQ(
        "g.test.timer.active 0 100\n"
        "g.test.timer.rate.one_sec 0 100\n"
        "g.test.timer.rate.one_min 1 100\n"
        "g.test.timer.rate.quarter_hour 1 100\n"
        "g.test.timer.rate.one_hour 1 100\n"
        "g.test.timer.timings.q50 1 100\n"
        "g.test.timer.timings.q80 1 100\n"
        "g.test.timer.timings.q90 1 100\n"
        "g.test.timer.timings.q95 1 100\n"
        "g.test.timer.timings.q99 1 100\n",
        p2.result());
}
