//
// Created by micha on 10.01.2026.
//

#ifndef NETSIM_NODES_HPP
#define NETSIM_NODES_HPP

#include "types.hpp"
#include "package.hpp"

// Typy odbiorców wymagane do badania spójności sieci
enum class ReceiverType {
    WORKER,
    STOREHOUSE
};

// Interfejs dla każdego elementu, który może przyjąć paczkę
class IPackageReceiver {
public:

    // Używamy Package&& p, czyli przenoszenia.
    virtual void receive_package(Package&& p) = 0;

    virtual ElementID get_id() const = 0;

    // Metoda potrzebna do algorytmu sprawdzania, czy fabryka jest dobrze zbudowana
    virtual ReceiverType get_receiver_type() const = 0;

    virtual ~IPackageReceiver() = default;
};
#endif //NETSIM_NODES_HPP