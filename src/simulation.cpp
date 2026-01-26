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