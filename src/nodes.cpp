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
    if (buffer_ && !receivers_.empty()) {

        // Wybieramy odbiorcę z jednakowym prawdopodobieństwem (wymóg z instrukcji)
        size_t index = std::rand() % receivers_.size();
        IPackageReceiver* receiver = receivers_[index];

        // Przekazujemy paczkę (move semantics!) i czyścimy bufor
        receiver->receive_package(std::move(buffer_.value()));
        buffer_.reset();
    }
}