//
// Lukasz 15.01.2026
//

#include "nodes.hpp"
#include "worker.hpp"
#include "storehouse.hpp"
#include <cstdlib>

void Worker::do_work(Time t) {
    // 1. Jeśli nie pracujemy, a kolejka nie jest pusta - pobierz paczkę
    if (!processingbuffer && !q_->empty()) {
        processingbuffer = q->pop();
        startTime = t;
    }

    // 2. Jeśli pracujemy, sprawdź czy czas minął
    if (processingbuffer) {
        // Czas pracy to różnica między aktualnym czasem a czasem rozpoczęcia.
        // Jeśli pd = 1, paczka powinna wyjść w tym samym kwancie czasu.
        if (t - startTime + 1 >= pd_) {
            push_package(std::move(*processingbuffer));
            processingbuffer.reset();
        }
    }
}
//
//
//
