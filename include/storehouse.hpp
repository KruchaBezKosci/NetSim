//
// Lukasz 15.01.2026
//
#ifndef NETSIM_STOREHOUSE_HPP
#define NETSIM_STOREHOUSE_HPP

#include "nodes.hpp"
#include "storage_types.hpp"
#include <memory>


class Storehouse : public IPackageReceiver {
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageQueue> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO))
        : id_(id), d_(std::move(d)) {}

    void receive_package(Package&& p) override { d_->push(std::move(p)); }
    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override { return ReceiverType::STOREHOUSE; }

    IPackageStockpile::const_iterator begin() const override { return d_->begin(); }
    IPackageStockpile::const_iterator end() const override { return d_->end(); }

private:
    ElementID id_;
    std::unique_ptr<IPackageQueue> d_;
};
//
//
//
