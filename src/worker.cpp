//
// Lukasz 15.01.2026
//

#include "worker.hpp"

void Worker::do_work(Time t) {
    // 1. Jeśli bufor pusty i mamy coś w kolejce - zacznij pracę
    if (!processing_buffer_ && !q_->empty()) {
        processing_buffer_ = q_->pop();
        start_time_ = t;
    }

    // 2. Jeśli pracujemy, sprawdź czy czas przetwarzania upłynął
    if (processing_buffer_) {
        if (t - start_time_ + 1 >= pd_) {
            push_package(std::move(*processing_buffer_));
            processing_buffer_.reset();
            start_time_ = 0;
        }
    }
}

