//
// Created by pawel on 22.01.2026.
//

#ifndef REPORTS_HPP
#define REPORTS_HPP

#include "factory.hpp"
#include <iostream>

void generate_structure_report(const Factory& f, std::ostream& os);

void generate_simulation_turn_report(const Factory& f, std::ostream& os, int turn_number);

#endif //REPORTS_HPP
