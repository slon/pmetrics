#include <pm/metrics.h>

#include <mutex>
#include <atomic>
#include <map>

#include <pm/tree.h>

namespace pm {

struct gauge_impl_t : public tree_leaf_t {
	virtual void print(tree_printer_t* printer) {
		if(int_gauge_) {
			printer->value(int_gauge_());
		} else {
			printer->value(double_gauge_());
		}
	}

	std::function<int64_t()> int_gauge_;
	std::function<double()> double_gauge_;
};

struct counter_impl_t : public tree_leaf_t {
	virtual void print(tree_printer_t* printer) {
		printer->value(value_);
	}

	std::atomic<int64_t> value_;
};

struct meter_impl_t : public tree_leaf_t {
	virtual void print(tree_printer_t* printer) {

	}
};

struct histogram_impl_t : public tree_leaf_t {
	virtual void print(tree_printer_t* printer) {

	}
};

struct timer_impl_t : public tree_leaf_t {
	virtual void print(tree_printer_t* printer) {

	}
};

struct health_check_impl_t : public tree_leaf_t {
	virtual void print(tree_printer_t* printer) {

	}
};

struct metric_registry_impl_t : public tree_branch_t {
};

} // namespace pm
