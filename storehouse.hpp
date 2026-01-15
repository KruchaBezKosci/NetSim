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
//
//
//
