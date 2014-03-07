#include <pm/metrics.h>

#include <atomic>

namespace pm {

struct metric_printer_t {
	void print(double value);
	void print(int64_t value);
	void print(const std::string& key, double value);
	void print(const std::string& key, int64_t value);
};

struct metric_impl_t {
	virtual void print(metric_printer_t* printer) = 0;

	virtual ~metric_impl_t() {}
};

struct gauge_impl_t : public metric_impl_t {
	virtual void print(metric_printer_t* printer) {
		if(int_gauge_) {
			printer->print(int_gauge_());
		} else {
			printer->print(double_gauge_());
		}
	}

	std::function<int64_t()> int_gauge_;
	std::function<double()> double_gauge_;
};

struct counter_impl_t : public metric_impl_t {
	virtual void print(metric_printer_t* printer) {
		printer->print(value_);
	}

	std::atomic<int64_t> value_;
};

struct meter_impl_t : public metric_impl_t {
	virtual void print(metric_printer_t* printer) {

	}
};

struct histogram_impl_t : public metric_impl_t {
	virtual void print(metric_printer_t* printer) {

	}
};

struct timer_impl_t : public metric_impl_t {
	virtual void print(metric_printer_t* printer) {

	}
};

struct health_check_impl_t : public metric_impl_t {
	virtual void print(metric_printer_t* printer) {

	}
};

} // namespace pm
