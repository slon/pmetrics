#include <pm/metrics.h>

#include <mutex>
#include <atomic>
#include <map>

#include <pm/tree.h>
#include <pm/counter.h>

namespace pm {

struct counter_impl_t : public tree_leaf_t {
	virtual void print(tree_printer_t* printer) {
		printer->value(value);
	}

	std::atomic<int64_t> value;
};

void counter_t::inc(int64_t amount) {
	if(impl_) {
		impl_->value += amount;
	}
}

void counter_t::dec(int64_t amount) {
	if(impl_) {
		impl_->value -= amount;
	}
}

void counter_t::set(int64_t value) {
	if(impl_) {
		impl_->value = value;
	}
}

struct meter_impl_t : public tree_leaf_t {
	virtual void print(tree_printer_t* printer) {
		printer->start_node();

		printer->child("one_sec");
		printer->value(one_sec.value());

		printer->child("one_min");
		printer->value(one_min.value());

		printer->child("one_hour");
		printer->value(one_hour.value());

		printer->end_node();
	}

	double_buffer_count_t one_sec;
	decaying_count_t one_min, one_hour;
};

void meter_t::mark() {
	if(impl_) {
		// TODO update
	}
}

struct histogram_impl_t : public tree_leaf_t {
	virtual void print(tree_printer_t* printer) {

	}

	std::vector<decaying_count_t> histogram;
};

struct timer_impl_t : public tree_leaf_t {
	virtual void print(tree_printer_t* printer) {

	}

	counter_impl_t active_count;
	meter_impl_t rate;
	histogram_impl_t five_min_timings;
};

metric_registry_t::metric_registry_t() : tree_(nullptr) {}
metric_registry_t::metric_registry_t(std::shared_ptr<tree_branch_t> branch) : tree_(branch) {}

metric_registry_t metric_registry_t::subtree(const std::string& name) {
	if(tree_) {
		return metric_registry_t(tree_->get_branch(name));
	} else {
		return metric_registry_t(nullptr);
	}
}

std::shared_ptr<tree_branch_t> metric_registry_t::ROOT = std::make_shared<tree_branch_t>();

metric_registry_t metric_registry_t::get_root() {
	return metric_registry_t(ROOT);
}

} // namespace pm
