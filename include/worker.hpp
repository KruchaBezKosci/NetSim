//
// Lukasz 15.01.2026
//

#ifndef NETSIM_WORKER_HPP
#define NETSIM_WORKER_HPP

#include <memory>
#include <optional>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include "types.hpp"
#include "package.hpp"
#include "storage_types.hpp"



class Worker : public PackageSender, public IPackageReceiver {
public:
    Worker(ElementID id, TimeOffset pd, std::uniqueptr<IPackageQueue> q)
        : PackageSender(), id(id), pd(pd), q(std::move(q)) {}

    void do_work(Time t);
    void receivepackage(Package&& p) override { q->push(std::move(p)); }
    ElementID getid() const override { return id; }

    // Metody specyficzne dla Worker
    ReceiverType get_receiver_type() const { return ReceiverType::WORKER; }
    IPackageStockpile::constiterator begin() const { return q->begin(); }
    IPackageStockpile::constiterator end() const { return q->end(); }
    IPackageStockpile::constiterator cbegin() const { return q->cbegin(); }
    IPackageStockpile::constiterator cend() const { return q->cend(); }

private:
    ElementID id;
    TimeOffset pd;
    std::uniqueptr<IPackageQueue> q;
    std::optional<Package> processingbuffer;
    Time startTime_ = 0;
};
//
//
//
#endif
