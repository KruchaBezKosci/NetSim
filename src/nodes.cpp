//
// Created by micha on 11.01.2026.
//
#include "nodes.hpp"
#include <cstdlib> // do std::rand()

void PackageSender::send_package() {
    // Sprawdzamy, czy w ogóle mamy paczkę do wysłania i czy mamy dokąd ją wysłać
    if (buffer_ && !receivers_.empty()) {

        // Wybieramy odbiorcę z jednakowym prawdopodobieństwem (wymóg z instrukcji)
        //
        size_t index = std::rand() % receivers_.size();
        IPackageReceiver* receiver = receivers_[index];

        // Przekazujemy paczkę (move semantics!) i czyścimy bufor
        receiver->receive_package(std::move(buffer_.value()));
        buffer_.reset();
    }
}