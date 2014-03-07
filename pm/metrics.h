#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <functional>
#include <chrono>

namespace pm {

typedef std::chrono::system_clock::duration duration_t;
typedef std::chrono::system_clock::time_point time_point_t;

struct gauge_impl_t;
struct counter_impl_t;
struct meter_impl_t;
struct histogram_impl_t;
struct timer_impl_t;
struct health_check_impl_t;

// instantaneous measurement of value
struct gauge_t {
	// private
	std::shared_ptr<gauge_impl_t> impl_;
};


// gauge over atomic integer
struct counter_t {
	void inc(int64_t amount = 1);
	void dec(int64_t amount = 1);

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
	struct dummy_delete_t { void operator() (timer_t*) {} };
	std::unique_ptr<timer_t, dummy_delete_t> timer_;
	time_point_t start_time_;
};

// measure the rate that a particular piece of code is called and the distribution of its duration
struct timer_t {
	timer_context_t time() { return timer_context_t(this); }

	time_point_t start();
	void update(time_point_t start_time);

	// private
	std::shared_ptr<meter_impl_t> rate_;
	std::shared_ptr<histogram_impl_t> timings_;
	std::shared_ptr<counter_impl_t> active_count_;
};

typedef std::unique_ptr<std::string> health_check_result_t;

struct health_check_t {
	// private
	std::shared_ptr<health_check_impl_t> impl_;
};

template<class metric_t>
class named_t {};

// collection of metrics
class metric_registry_t {
public:
	metric_registry_t with_prefix(const std::string& prefix);

	counter_t counter(const std::string& name);
	meter_t meter(const std::string& name);
	histogram_t histogram(const std::string& name, int min, int max, int precision);
	timer_t timer(const std::string& name);

	gauge_t gauge(const std::string& name, std::function<int64_t()> gauge);
	gauge_t gauge(const std::string& name, std::function<double()> gauge);

	health_check_t health_check(const std::string& name, std::function<health_check_result_t()> check);

	template<class metric_t>
	named_t<metric_t> named(const std::string& name);

private:
	class metrics_registry_impl_t;

	std::string prefix_;
	std::shared_ptr<metrics_registry_impl_t> impl_;
};

metric_registry_t get_root();

} // namespace pm
