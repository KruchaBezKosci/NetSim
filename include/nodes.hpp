//
// Created by micha on 10.01.2026.
//

#ifndef NETSIM_NODES_HPP
#define NETSIM_NODES_HPP

#include "types.hpp"
#include "package.hpp"

#include <optional>
#include <vector>

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

class PackageSender {
public:
    PackageSender() = default;
    PackageSender(PackageSender&&) = default;

    // Metoda do dodawania odbiorcy (np. konkretnego robotnika do listy rampy)
    void add_receiver(IPackageReceiver* receiver) { receivers_.push_back(receiver); }

    // Metoda do sprawdzania, komu rampa może wysłać paczkę
    const std::vector<IPackageReceiver*>& get_receivers() const { return receivers_; }

    // Tu będzie się działo wysyłanie (logikę napiszemy w .cpp)
    void send_package();

    // Dostęp do bufora paczki (żeby sprawdzić, co rampa aktualnie "trzyma")
    const std::optional<Package>& get_sending_buffer() const { return buffer_; }

protected:
    // Tu rampa umieszcza paczkę, zanim zostanie ona przekazana dalej
    void push_package(Package&& p) { buffer_ = std::move(p); }

    std::optional<Package> buffer_; // Bufor na jedną paczkę

private:
    std::vector<IPackageReceiver*> receivers_; // Lista potencjalnych odbiorców
};


#endif //NETSIM_NODES_HPP