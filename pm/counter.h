#pragma once

#include <atomic>
#include <mutex>

#include <pm/time.h>

namespace pm {

struct spinlock_t {
	spinlock_t() : locked_(false) {}

	void lock() {
		while(!locked_.test_and_set()) {}
	}

	void unlock() {
		locked_.clear();
	}

	std::atomic_flag locked_;
};

class double_buffer_counter_t {
public:
	double_buffer_counter_t(duration_t window_size) :
		window_size_(window_size),
		value_(0), next_value_(0),
		next_swap_(std::chrono::system_clock::now() + window_size_) {}

	int64_t value(time_point_t at) {
		std::lock_guard<spinlock_t> guard(lock_);
		maybe_swap(at);

		return value_;
	}

	void mark(time_point_t at) {
		std::lock_guard<spinlock_t> guard(lock_);
		maybe_swap(at);

		++next_value_;
	}

private:
	const duration_t window_size_;

	spinlock_t lock_;
	int64_t value_, next_value_;
	time_point_t next_swap_;

	void maybe_swap(time_point_t at) {
		if(at > next_swap_) {
			value_ = next_value_;
			next_value_ = 0;
			next_swap_ = at + window_size_;
		}		
	}
};

class decaying_counter_t {
public:
	int64_t value(time_point_t at) { return 0; }

private:
	
};

} // namespace pm
