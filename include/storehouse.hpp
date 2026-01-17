//
// Lukasz 15.01.2026
//
#ifndef NETSIM_STOREHOUSE_HPP
#define NETSIM_STOREHOUSE_HPP

#include <memory>
#include <optional>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include "types.hxx"
#include "package.hxx"
#include "storage_types.hxx"


class Storehouse : public IPackageReceiver {
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageQueue> d = std::makeunique<PackageQueue>(PackageQueueType::FIFO))
        : id(id), d_(std::move(d)) {}

    void receivepackage(Package&& p) override { d->push(std::move(p)); }
    ElementID getid() const override { return id; }

    // Metody specyficzne dla Storehouse
    ReceiverType get_receiver_type() const { return ReceiverType::STOREHOUSE; }
    IPackageStockpile::constiterator begin() const { return d->begin(); }
    IPackageStockpile::constiterator end() const { return d->end(); }
    IPackageStockpile::constiterator cbegin() const { return d->cbegin(); }
    IPackageStockpile::constiterator cend() const { return d->cend(); }

private:
    ElementID id_;
    std::uniqueptr<IPackageQueue> d;
};
//
//
//
