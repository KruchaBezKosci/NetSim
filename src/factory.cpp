// created by Krok 19.01


#include "factory.hpp"

// ---------- DFS consistency ----------
bool has_reachable_storehouse(const PackageSender* sender,
                              std::map<const PackageSender*, NodeColor>& node_colors) {
    auto it = node_colors.find(sender);
    if (it != node_colors.end() && it->second == NodeColor::VERIFIED) {
        return true; // już zweryfikowany
    }

    node_colors[sender] = NodeColor::VISITED;

    const auto& prefs = sender->receiver_preferences_.get_preferences();
    if (prefs.empty()) {
        throw std::logic_error("Sender has no outgoing connections.");
    }

    bool has_receiver_other_than_self = false;
    bool reachable_storehouse = false;

    for (const auto& [receiver_ptr, _prob] : prefs) {
        if (!receiver_ptr) continue;

        if (receiver_ptr->get_receiver_type() == ReceiverType::STOREHOUSE) {
            has_receiver_other_than_self = true;
            reachable_storehouse = true;
            continue;
        }

        // ReceiverType::WORKER -> downcast do Worker*, potem upcast do PackageSender*
        auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
        if (!worker_ptr) {
            throw std::logic_error("ReceiverType=WORKER but receiver is not Worker.");
        }

        auto sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);
        if (!sendrecv_ptr) {
            throw std::logic_error("Worker is not a PackageSender.");
        }

        // self-loop ignorujemy
        if (sendrecv_ptr == sender) {
            continue;
        }

        has_receiver_other_than_self = true;

        if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED) {
            if (has_reachable_storehouse(sendrecv_ptr, node_colors)) {
                reachable_storehouse = true;
            }
        } else if (node_colors[sendrecv_ptr] == NodeColor::VERIFIED) {
            reachable_storehouse = true;
        }
    }

    node_colors[sender] = NodeColor::VERIFIED;

    if (has_receiver_other_than_self && reachable_storehouse) {
        return true;
    }

    throw std::logic_error("No reachable storehouse from sender.");
}

// ============================================================
// Factory
// ============================================================
bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> colors;

    // Inicjalizacja kolorów dla ramp i workerów
    for (const auto& r : ramps_) {
        colors[static_cast<const PackageSender*>(&r)] = NodeColor::UNVISITED;
    }
    for (const auto& w : workers_) {
        colors[static_cast<const PackageSender*>(&w)] = NodeColor::UNVISITED;
    }

    try {
        for (const auto& r : ramps_) {
            has_reachable_storehouse(static_cast<const PackageSender*>(&r), colors);
        }
    } catch (const std::logic_error&) {
        return false;
    }

    return true;
}

// ============================================================
// Metody symulacji (core)
// ============================================================
void Factory::do_deliveries(Time t) {
    for (auto& r : ramps_) {
        r.deliver_goods(t);
    }
}

void Factory::do_package_passing() {
    for (auto& r : ramps_) {
        r.send_package();
    }
    for (auto& w : workers_) {
        w.send_package();
    }
}

void Factory::do_work(Time t) {
    for (auto& w : workers_) {
        w.do_work(t);
    }
}
