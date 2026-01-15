//
// Created by micha on 11.01.2026.
//
#include "nodes.hpp"
#include <cstdlib> // do std::rand()


void ReceiverPreferences::add_receiver(IPackageReceiver* r) {
    preferences_[r] = 0.0;
    rescale();
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r) {
    preferences_.erase(r);
    rescale();
}

void ReceiverPreferences::rescale() {
    if (preferences_.empty()) return;
    double p = 1.0 / static_cast<double>(preferences_.size());
    for (auto& pair : preferences_) {
        pair.second = p;
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    double val = pg_();
    double sum = 0.0;
    for (auto const& [receiver, prob] : preferences_) {
        sum += prob;
        if (val <= sum) return receiver;
    }
    return nullptr;
}

void PackageSender::send_package() {
    // Sprawdzamy, czy w ogóle mamy paczkę do wysłania i czy mamy dokąd ją wysłać
    if (buffer_) {
        IPackageReceiver* r = receiver_preferences_.choose_receiver();
        if (r) {
            r->receive_package(std::move(*buffer_));
            buffer_.reset();
        }
    }
}
//
// Lukasz 15.01.2026
//
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