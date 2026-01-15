//
// Lukasz 15.01.2026
//

#include "nodes.hpp"
#include <cstdlib> // do std::rand()

void Worker::do_work(Time t) {
   
    if (!processing_buffer_.has_value() && !q_->empty()) {

        processing_buffer_.emplace(q_->pop());

        startTime_ = t;
    }

    if (processing_buffer_.has_value()) {

        if (t - startTime_ + 1 >= pd_) {

            push_package(std::move(processing_buffer_.value()));

            processing_buffer_.reset();
            startTime_ = 0;
        }
    }
}
//
//
//
