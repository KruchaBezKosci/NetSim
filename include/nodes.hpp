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

    virtual ~IPackageReceiver() = default;
};

class PackageSender {
public:
    PackageSender() = default;
    PackageSender(PackageSender&&) = default;

    auto begin() { return receivers_.begin(); }
    auto end() { return receivers_.end(); }
    auto cbegin() const { return receivers_.cbegin(); }
    auto cend() const { return receivers_.cend(); }

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

//
// Lukasz 15.01.2026
//
class Storehouse : public IPackageReceiver {
public:

    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO))
        : id_(id), d_(std::move(d)) {}

    void receive_package(Package&& p) override {
        d_->push(std::move(p));
    }

    ElementID get_id() const override { return id_; }
    
    ReceiverType get_receiver_type() const override { return ReceiverType::STOREHOUSE; }

    IPackageStockpile* get_stockpile() const { return d_.get(); }

    auto begin() const override { return d_->begin(); }
    auto end() const override { return d_->end(); }
    auto cbegin() const { return d_->cbegin(); }
    auto cend() const { return d_->cend(); }

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};

class Worker : public PackageSender, public IPackageReceiver {
public:

    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
        : PackageSender(), id_(id), pd_(pd), q_(std::move(q)) {}

    void receive_package(Package&& p) override { q_->push(std::move(p)); }
    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; }

    void do_work(Time t);

    TimeOffset get_processing_duration() const { return pd_; }
    Time get_package_processing_start_time() const { return startTime_; }
    IPackageQueue* get_queue() const { return q_.get(); }
    
    const std::optional<Package>& get_processing_buffer() const { return processing_buffer_; }

    IPackageStockpile::const_iterator begin() const override { return q_->begin(); }
    IPackageStockpile::const_iterator end() const override { return q_->end(); }

private:
    ElementID id_;
    TimeOffset pd_;
    std::unique_ptr<IPackageQueue> q_;
    std::optional<Package> processing_buffer_; 
    Time startTime_ = 0;
};
//
//
//

#endif //NETSIM_NODES_HPP