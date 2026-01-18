//
// Lukasz 15.01.2026
//

#ifndef NETSIM_WORKER_HPP
#define NETSIM_WORKER_HPP

#include <memory>
#include <optional>
#include "types.hpp"
#include "package.hpp"
#include "storage_types.hpp"
#include "nodes.hpp" // Zawiera PackageSender i IPackageReceiver

class Worker : public PackageSender, public IPackageReceiver {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
        : id_(id), pd_(pd), q_(std::move(q)) {}

    void do_work(Time t);

    void receive_package(Package&& p) override { q_->push(std::move(p)); }
    ElementID get_id() const override { return id_; }

    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; }

    IPackageStockpile::const_iterator begin() const override { return q_->begin(); }
    IPackageStockpile::const_iterator end() const override { return q_->end(); }
    IPackageStockpile::const_iterator cbegin() const override { return q_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return q_->cend(); }

    // Dodatkowe metody zgodne ze schematem UML
    TimeOffset get_processing_duration() const { return pd_; }
    Time get_package_processing_start_time() const { return start_time_; }

private:
    ElementID id_;
    TimeOffset pd_;
    std::unique_ptr<IPackageQueue> q_;
    std::optional<Package> processing_buffer_;
    Time start_time_ = 0;
};

#endif