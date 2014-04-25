#include <pm/metrics.h>
#include <pm/graphite.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;
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

static const std::string FLOAT_RE = "[0-9]+(.[0-9]+)?";

TEST(metrics_test_t, meter) {
    meter_t m = get_root().subtree("test").meter("meter");

    m.mark();

    graphite_printer_t p("g", 100);
    get_root().print(&p);

    EXPECT_THAT(p.result(), MatchesRegex(
        "g.test.meter.one_sec " + FLOAT_RE + " 100\n"
        "g.test.meter.one_min " + FLOAT_RE + " 100\n"
        "g.test.meter.quarter_hour " + FLOAT_RE + " 100\n"
        "g.test.meter.one_hour " + FLOAT_RE + " 100\n"
    ));
}

TEST(metrics_test_t, timer) {
    pm::timer_t t = get_root().subtree("test").timer("timer");

    auto token = t.time();

    graphite_printer_t p("g", 100);
    get_root().print(&p);

    EXPECT_THAT(p.result(), MatchesRegex(
        "g.test.timer.active 1 100\n"
        "g.test.timer.rate.one_sec " + FLOAT_RE + " 100\n"
        "g.test.timer.rate.one_min " + FLOAT_RE + " 100\n"
        "g.test.timer.rate.quarter_hour " + FLOAT_RE + " 100\n"
        "g.test.timer.rate.one_hour " + FLOAT_RE + " 100\n"
        "g.test.timer.timings.q50 " + FLOAT_RE + " 100\n"
        "g.test.timer.timings.q80 " + FLOAT_RE + " 100\n"
        "g.test.timer.timings.q90 " + FLOAT_RE + " 100\n"
        "g.test.timer.timings.q95 " + FLOAT_RE + " 100\n"
        "g.test.timer.timings.q99 " + FLOAT_RE + " 100\n"
    ));

    token.finish();

    graphite_printer_t p2("g", 100);
    get_root().print(&p2);

    EXPECT_THAT(p2.result(), MatchesRegex(
        "g.test.timer.active 0 100\n" // active should be zero
        "g.test.timer.rate.one_sec " + FLOAT_RE + " 100\n"
        "g.test.timer.rate.one_min " + FLOAT_RE + " 100\n"
        "g.test.timer.rate.quarter_hour " + FLOAT_RE + " 100\n"
        "g.test.timer.rate.one_hour " + FLOAT_RE + " 100\n"
        "g.test.timer.timings.q50 " + FLOAT_RE + " 100\n"
        "g.test.timer.timings.q80 " + FLOAT_RE + " 100\n"
        "g.test.timer.timings.q90 " + FLOAT_RE + " 100\n"
        "g.test.timer.timings.q95 " + FLOAT_RE + " 100\n"
        "g.test.timer.timings.q99 " + FLOAT_RE + " 100\n"
    ));
}
