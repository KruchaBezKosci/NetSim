//
// Lukasz 15.01.2026
//

#ifndef NETSIM_NODES_HPP
#define NETSIM_NODES_HPP

#include "types.hpp"
#include "package.hpp"
#include "storage_types.hpp"

#include <optional>
#include <vector>
#include <memory>


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
