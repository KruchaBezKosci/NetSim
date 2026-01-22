//
// Created by pawel on 04.01.2026.
//

#include <iostream>
#include "factory.hpp"
#include "reports.hpp"
#include "nodes.hpp"
#include "package.hpp"
#include "ramp.hpp"
#include "helpers.hpp"
#include "storage_types.hpp"
#include "storehouse.hpp"
#include "worker.hpp"


int main() {
    Factory f;

    f.add_ramp(Ramp(1, 2));


    f.add_worker(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));


    f.add_storehouse(Storehouse(1));

    auto r = f.find_ramp_by_id(1);
    auto w = f.find_worker_by_id(1);
    auto s = f.find_storehouse_by_id(1);

    r->receiver_preferences_.add_receiver(&(*w));
    w->receiver_preferences_.add_receiver(&(*s));
    std::cout << "--- TEST RAPORTU STRUKTURY ---\n";
    generate_structure_report(f, std::cout);
    std::cout << "\n--- TEST RAPORTU STANU (Tura 1) ---\n";
    generate_simulation_turn_report(f, std::cout, 1);

    return 0;
}