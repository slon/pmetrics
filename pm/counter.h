#pragma once

#include <atomic>
#include <cmath>
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
	explicit double_buffer_counter_t(duration_t window_size) :
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
    explicit decaying_counter_t (double decay_factor) :
        decay_factor_ (decay_factor),
        value_(0),
        last_(std::chrono::system_clock::now())
    {}

	uint64_t value (time_point_t at) {
        std::lock_guard<spinlock_t> guard(lock_);
        decay(at);
        return value_;
    }

    void mark (time_point_t at) {
        std::lock_guard<spinlock_t> guard(lock_);
        decay(at);
        value_ += 1;
    }

private:
    void decay(time_point_t at) {
        value_ *= exp(- decay_factor_ * double(std::chrono::duration_cast<std::chrono::seconds>(at - last_).count()));
    }

    double decay_factor_;
    uint64_t value_;

    time_point_t last_;

	spinlock_t lock_;
};

} // namespace pm
