//
// Created by micha on 26.01.2026.
//
#include "simulation.hpp"

// Logika dla raportowania co interwał (np. co 2 tury: 1, 3, 5...)
bool IntervalReportNotifier::should_generate_report(Time t) {
    // Pierwszy raport zawsze w turze 1, kolejne co 'to_' tur
    return (t - 1) % to_ == 0;
}

// Logika dla raportowania w konkretnych turach
bool SpecificTurnsReportNotifier::should_generate_report(Time t) {
    // Sprawdzamy, czy aktualna tura 't' znajduje się w zbiorze zdefiniowanych tur
    return turns_.find(t) != turns_.end();
}

void simulate(Factory& f, TimeOffset turns, std::function<void(Factory&, Time)> rf) {
    // 1. Sprawdzenie spójności sieci przed rozpoczęciem symulacji
    if (!f.is_consistent()) {
        throw std::logic_error("Network is not consistent - unreachable storehouse from ramp.");
    }

    // 2. Główna pętla symulacji
    for (Time t = 1; t <= turns; ++t) {
        // Kolejność kroków zgodnie ze specyfikacją:
        f.do_deliveries(t);       // Rampa generuje paczkę i wrzuca do bufora
        f.do_package_passing();   // Paczki z buforów wędrują do odbiorców
        f.do_work(t);             // Robotnicy pobierają paczki i przetwarzają

        // 3. Wywołanie funkcji raportującej (Strategy)
        if (rf) {
            rf(f, t);
        }
    }
}