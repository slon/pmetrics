#include <pm/metrics.h>

#include <mutex>
#include <atomic>
#include <map>

#include <pm/tree.h>
#include <pm/counter.h>

namespace pm {

struct counter_impl_t : public tree_leaf_t {
    virtual void print(tree_printer_t* printer) { printer->value(value); }

    std::atomic<int64_t> value;
};

void counter_t::inc(int64_t amount) {
    if (impl_) impl_->value += amount;
}

void counter_t::dec(int64_t amount) {
    if (impl_) impl_->value -= amount;
}

void counter_t::set(int64_t value) {
    if (impl_) impl_->value = value;
}

struct meter_impl_t : public tree_leaf_t {

    meter_impl_t()
        : one_sec(std::chrono::seconds(1)),
          one_min(std::chrono::seconds(60)),
          quarter_hour(std::chrono::seconds(15 * 60)),
          one_hour(std::chrono::seconds(60 * 60)) {}

    virtual void print(tree_printer_t* printer) {
        printer->start_node();

        auto now = std::chrono::system_clock::now();

        printer->child("one_sec");
        printer->value(one_sec.value(now));

        printer->child("one_min");
        printer->value(one_min.value(now));

        printer->child("quarter_hour");
        printer->value(quarter_hour.value(now));

        printer->child("one_hour");
        printer->value(one_hour.value(now));

        printer->end_node();
    }

    virtual void mark() {
        auto now = std::chrono::system_clock::now();

        one_sec.mark(now);
        one_min.mark(now);
        quarter_hour.mark(now);
        one_hour.mark(now);
    }

    double_buffer_counter_t one_sec;
    decaying_counter_t one_min, quarter_hour, one_hour;
};

void meter_t::mark() {
    if (impl_) {
        impl_->mark();
    }
}

struct histogram_impl_t : public tree_leaf_t {
    virtual void print(tree_printer_t* printer) {}

    std::vector<decaying_counter_t> histogram;
};

void histogram_t::update(int64_t value) {
    if (impl_) {
        // TODO
    }
}

struct timer_impl_t : public tree_leaf_t {
    virtual void print(tree_printer_t* printer) {}

    std::atomic<int64_t> active_count;
    meter_impl_t rate;
    histogram_impl_t five_min_timings;
};

timer_context_t::timer_context_t(timer_t* timer)
    : timer_(timer), start_time_(timer->start()) {}

void timer_context_t::finish() {
    if (timer_) {
        timer_->finish(start_time_);
        timer_ = nullptr;
    }
}

time_point_t timer_t::start() {
    if (impl_) {
        impl_->active_count += 1;

        return std::chrono::system_clock::now();
    } else {
        return time_point_t();
    }
}

void timer_t::finish(time_point_t start_time) {
    if (impl_) {
        impl_->active_count -= 1;
    }
}

metric_registry_t::metric_registry_t() : tree_(nullptr) {}
metric_registry_t::metric_registry_t(std::shared_ptr<tree_branch_t> branch)
    : tree_(branch) {}

metric_registry_t metric_registry_t::subtree(const std::string& name) {
    if (tree_) {
        return metric_registry_t(tree_->get_branch(name));
    } else {
        return metric_registry_t(nullptr);
    }
}

counter_t metric_registry_t::counter(const std::string& name) {
    if (tree_) {
        auto counter_impl = std::make_shared<counter_impl_t>();
        tree_->add_leaf(name, counter_impl);
        counter_t counter;
        counter.impl_ = counter_impl;
        return counter;
    } else {
        return counter_t();
    }
}

meter_t metric_registry_t::meter(const std::string& name) {
    if (tree_) {
        auto meter_impl = std::make_shared<meter_impl_t>();
        tree_->add_leaf(name, meter_impl);
        meter_t meter;
        meter.impl_ = meter_impl;
        return meter;
    } else {
        return meter_t();
    }
}

void metric_registry_t::print(tree_printer_t* printer) {
    if (tree_) {
        tree_->print(printer);
    }
}

std::shared_ptr<tree_branch_t> metric_registry_t::ROOT =
    std::make_shared<tree_branch_t>();

metric_registry_t metric_registry_t::get_root() {
    return metric_registry_t(ROOT);
}

}  // namespace pm
