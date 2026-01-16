//
// Lukasz 15.01.2026
//

#ifndef NETSIM_WORKER_HPP
#define NETSIM_WORKER_HPP

#include "nodes.hpp"
#include "storage_types.hpp"
#include <memory>



class Worker : public PackageSender, public IPackageReceiver {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
        : PackageSender(), id_(id), pd_(pd), q_(std::move(q)) {}

    void do_work(Time t); 

    void receive_package(Package&& p) override { q_->push(std::move(p)); }
    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; }

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
