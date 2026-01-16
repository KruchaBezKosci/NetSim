//
// Created by micha on 10.01.2026.
//

#ifndef NETSIM_NODES_HPP
#define NETSIM_NODES_HPP

#include "types.hpp"
#include "package.hpp"
#include "storage_types.hpp"

#include <optional>
#include <vector>
#include <list>
#include <memory>


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

    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;

    virtual ~IPackageReceiver() = default;
};

class PackageSender {
public:
    PackageSender() = default;
    PackageSender(PackageSender&&) = default;

    void send_package(); // Implementacja w .cpp
    const std::optional<Package>& get_sending_buffer() const { return buffer_; }
    void add_receiver(IPackageReceiver* receiver) { receivers_.push_back(receiver); }

    auto begin() { return receivers_.begin(); }
    auto end() { return receivers_.end(); }
    auto cbegin() const { return receivers_.cbegin(); }
    auto cend() const { return receivers_.cend(); }

protected:
    // Tu rampa umieszcza paczkę, zanim zostanie ona przekazana dalej
    void push_package(Package&& p) { buffer_ = std::move(p); }
    std::optional<Package> buffer_;
    std::vector<IPackageReceiver*> receivers_;

};


#endif //NETSIM_NODES_HPP