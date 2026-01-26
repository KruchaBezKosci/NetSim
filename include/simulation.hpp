//
// Created by micha on 26.01.2026.
//

#ifndef NETSIM_SIMULATION_HPP
#define NETSIM_SIMULATION_HPP

#include <functional>
#include <set>
#include <stdexcept> // Dla std::logic_error
#include "factory.hpp"
#include "types.hpp"

// Funkcja przeprowadzająca symulację
// f: Fabryka
// turns: Liczba tur (czas trwania symulacji)
// rf: Funkcja raportująca (Strategy Pattern)
void simulate(Factory& f, TimeOffset turns, std::function<void(Factory&, Time)> rf);

// Klasa zgłaszająca potrzebę raportu w równych odstępach czasu (co 'to' tur)
class IntervalReportNotifier {
public:
    explicit IntervalReportNotifier(TimeOffset to) : to_(to) {}
    bool should_generate_report(Time t);
private:
    TimeOffset to_;
};

// Klasa zgłaszająca potrzebę raportu w konkretnych, wskazanych turach
class SpecificTurnsReportNotifier {
public:
    explicit SpecificTurnsReportNotifier(std::set<Time> turns) : turns_(std::move(turns)) {}
    bool should_generate_report(Time t);
private:
    std::set<Time> turns_;
};

#endif //NETSIM_SIMULATION_HPP