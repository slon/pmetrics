#pragma once

#include <atomic>
#include <cmath>
#include <mutex>
#include <vector>

#include <pm/time.h>

namespace pm {

struct spinlock_t {
    spinlock_t() : locked_(false) {}
    spinlock_t(const spinlock_t& ) : locked_(false) {}

    void lock() {
        while (!locked_.test_and_set()) {}
    }

    void unlock() { locked_.clear(); }

    std::atomic_flag locked_;
};

class double_buffer_counter_t {
public:
    explicit double_buffer_counter_t(duration_t window_size)
        : window_size_(window_size),
          value_(0),
          next_value_(0),
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
        if (at > next_swap_) {
            value_ = next_value_;
            next_value_ = 0;
            next_swap_ = at + window_size_;
        }
    }
};

class decaying_counter_t {
public:
    explicit decaying_counter_t(duration_t decay_time)
        : decay_time_(decay_time),
          value_(0),
          last_(std::chrono::system_clock::now()) {}

    double value(time_point_t at) {
        std::lock_guard<spinlock_t> guard(lock_);
        decay(at);
        return value_;
    }

    void mark(time_point_t at) {
        std::lock_guard<spinlock_t> guard(lock_);
        decay(at);
        value_ += 1;
    }

private:
    void decay(time_point_t at) {
        if (at <= last_) return;
        double decay = (at - last_) / decay_time_;

        last_ = at;
        value_ *= exp(- decay);
    }

    duration_t decay_time_;
    double value_;

    time_point_t last_;

    spinlock_t lock_;
};

class linear_mapping_t {
public:
    linear_mapping_t(double min, double max, int n_buckets) : min_(min), max_(max), n_buckets_(n_buckets) {}

    int n_buckets() { return n_buckets_; }

    int map(double value) {
        int bucket = (value - min_) * n_buckets_ / (max_ - min_);

        if(bucket < 0) return 0;
        if(bucket >= n_buckets_) return n_buckets_ - 1;
        return bucket;
    }

    double unmap(int bucket_index) {
        return min_ + (max_ - min_) * bucket_index / n_buckets_;
    }

private:
    double min_, max_;
    int n_buckets_;
};

class histogram_counter_t {
public:
    histogram_counter_t(duration_t interval, linear_mapping_t mapping)
        : mapping_(mapping), histogram_(mapping.n_buckets(), decaying_counter_t(interval)), total_(interval) {}

    void update(time_point_t at, double value) {
        total_.mark(at);
        histogram_[mapping_.map(value)].mark(at);
    }

    void get_quantiles(time_point_t at, const std::vector<double>& quantiles, std::vector<double>* quantiles_value) {
        quantiles_value->resize(quantiles.size());

        double total = total_.value(at);
        double sum = 0.0;

        size_t q = 0;
        for(size_t i = 0; i < histogram_.size() && q < quantiles.size(); ++i) {
            while(q < quantiles.size() && sum  >= quantiles[q] * total) {
                (*quantiles_value)[q] = mapping_.unmap(i);
                ++q;
            }

            sum += histogram_[i].value(at);
        }

        for(; q < quantiles.size(); ++q) {
            (*quantiles_value)[q] = mapping_.unmap(mapping_.n_buckets());
        }
    }

private:
    linear_mapping_t mapping_;

    std::vector<decaying_counter_t> histogram_;
    decaying_counter_t total_;
};

}  // namespace pm
