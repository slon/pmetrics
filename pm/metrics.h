#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <functional>

#include <pm/time.h>

namespace pm {

struct counter_impl_t;
struct meter_impl_t;
struct histogram_impl_t;
struct timer_impl_t;

// instantaneous value of integer
struct counter_t {
    void inc(int64_t amount = 1);
    void dec(int64_t amount = 1);
    void set(int64_t value);

    // private
    std::shared_ptr<counter_impl_t> impl_;
};

// measure rate of events over time e.g. RPS
struct meter_t {
    void mark();

    // private
    std::shared_ptr<meter_impl_t> impl_;
};

// measure statistical distribution of data
struct histogram_t {
    void update(int64_t value);

    // private
    std::shared_ptr<histogram_impl_t> impl_;
};

struct timer_t;

class timer_context_t {
public:
    timer_context_t(timer_t* timer);
    ~timer_context_t() { finish(); }

    void finish();

private:
    struct dummy_delete_t {
        void operator()(timer_t*) {}
    };
    std::unique_ptr<timer_t, dummy_delete_t> timer_;
    time_point_t start_time_;
};

// measure the rate that a particular piece of code is called and the
// distribution of its duration
struct timer_t {
    timer_context_t time() { return timer_context_t(this); }

    time_point_t start();
    void finish(time_point_t start_time);

    // private
    std::shared_ptr<timer_impl_t> impl_;
};

template <class metric_t>
class named_t {};

class tree_branch_t;
class tree_printer_t;

// collection of metrics
class registry_t {
public:
    registry_t();
    registry_t(std::shared_ptr<tree_branch_t> branch);

    static registry_t get_root();

    registry_t subtree(const std::string& prefix);

    counter_t counter(const std::string& name);
    meter_t meter(const std::string& name);
    histogram_t histogram(const std::string& name, int min, int max);
    timer_t timer(const std::string& name);

    template <class metric_t>
    named_t<metric_t> named(const std::string& name);

    void print(tree_printer_t* printer);

private:
    std::shared_ptr<tree_branch_t> tree_;

    static std::shared_ptr<tree_branch_t> ROOT;
};

inline registry_t get_root() { return registry_t::get_root(); }

}  // namespace pm
