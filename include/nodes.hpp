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
#include <functional>
#include <map>


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

    [[nodiscard]] virtual ElementID get_id() const = 0;

    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;
    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;

    virtual ReceiverType get_receiver_type() const { return ReceiverType::WORKER; }

    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    ReceiverPreferences(std::function<double()> pg = []() { return (double)rand() / RAND_MAX; })
        : pg_(std::move(pg)) {}

    void add_receiver(IPackageReceiver* r);
    void remove_receiver(IPackageReceiver* r);
    IPackageReceiver* choose_receiver();

    [[nodiscard]] const preferences_t& get_preferences() const { return preferences_; }

    const_iterator begin() const { return preferences_.begin(); }
    const_iterator end() const { return preferences_.end(); }
    const_iterator cbegin() const { return preferences_.cbegin(); }
    const_iterator cend() const { return preferences_.cend(); }
    
private:
    preferences_t preferences_;
    std::function<double()> pg_;
    void rescale();
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