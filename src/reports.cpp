//
// Created by pawel on 22.01.2026.
//
#include "reports.hpp"
#include "storehouse.hpp"
#include <algorithm>
#include <vector>


std::string queue_type_to_string(PackageQueueType type) {
    return (type == PackageQueueType::FIFO) ? "FIFO" : "LIFO";
}
std::string receiver_type_to_string(ReceiverType type) {
    if (type == ReceiverType::WORKER) {
        return "WORKER";
    } else if (type == ReceiverType::STOREHOUSE) {
        return "STOREHOUSE";
    }
    return "UNKNOWN";
}


void generate_structure_report(const Factory& f, std::ostream& os) {

    os << "\n== LOADING RAMPS ==\n";

    for (auto it = f.ramp_cbegin(); it != f.ramp_cend(); ++it) {
        const auto& ramp = *it;
        os << "LOADING RAMP #" << ramp.get_id() << "\n";
        os << "  Delivery interval: " << ramp.get_delivery_interval() << "\n";
        os << "  Receivers:\n";
        for (const auto& [receiver, p] : ramp.receiver_preferences_) {
            os << "    " << receiver_type_to_string(receiver->get_receiver_type())
               << " #" << receiver->get_id() << "\n";
        }
    }
    os << "\n== WORKERS ==\n";
    for (auto it = f.worker_cbegin(); it != f.worker_cend(); ++it) {
        const auto& worker = *it;
        os << "WORKER #" << worker.get_id() << "\n";
        os << "  Processing time: " << worker.get_processing_time() << "\n";
        os << "  Queue type: " << queue_type_to_string(worker.get_queue()->get_queue_type()) << "\n";
        os << "  Receivers:\n";

        for (const auto& [receiver, p] : worker.receiver_preferences_) {
            os << "    " << receiver_type_to_string(receiver->get_receiver_type())
               << " #" << receiver->get_id() << "\n";
        }
    }
    os << "\n== STOREHOUSES ==\n";
    for (auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); ++it) {
        const auto& storehouse = *it;
        os << "STOREHOUSE #" << storehouse.get_id() << "\n";
    }
    os << "\n";
}


void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t) {
    os << "=== [ Turn: " << t << " ] ===\n";
    // 1. Stan Robotników
    os << "\n== WORKERS ==\n";
    for (auto it = f.worker_cbegin(); it != f.worker_cend(); ++it) {
        const auto& worker = *it;
        os << "WORKER #" << worker.get_id() << "\n";

        const auto& processing_buffer = worker.get_processing_buffer();

        if (processing_buffer.has_value()) {
            os << "  PBuffer: Package #" << processing_buffer->get_id()
               << " (pt: " << (t - worker.get_package_processing_start_time() + 1) << ")\n";
        } else {
            os << "  PBuffer: (empty)\n";
        }
        os << "  Queue: ";
        const auto* queue = worker.get_queue();
        if (queue->empty()) {
            os << "(empty)";
        } else {

            for (auto q_it = queue->cbegin(); q_it != queue->cend(); ++q_it) {
                os << "#" << q_it->get_id();
                if (std::next(q_it) != queue->cend()) {
                    os << ", ";
                }
            }
        }
        os << "\n";
    }

    os << "\n== STOREHOUSES ==\n";
    for (auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); ++it) {
        const auto& storehouse = *it;
        os << "STOREHOUSE #" << storehouse.get_id() << "\n";

        os << "  Queue: ";
        const auto* queue = storehouse.get_queue();
        if (queue->empty()) {
            os << "(empty)";
        } else {
            for (auto q_it = queue->cbegin(); q_it != queue->cend(); ++q_it) {
                os << "#" << q_it->get_id();
                if (std::next(q_it) != queue->cend()) {
                    os << ", ";
                }
            }
        }
        os << "\n";
    }
    os << "\n";
}