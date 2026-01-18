//
// lukasz 17.01.26
// modified by Krok 18.01
//
#ifndef RAMP_HPP
#define RAMP_HPP

#include "types.hpp"
#include "package.hpp"
#include "nodes.hpp" // Zawiera PackageSender

class Ramp : public PackageSender {
public:
    Ramp(ElementID id, TimeOffset di) : id_(id), di_(di) {}

    void deliver_goods(Time t);

    ElementID get_id() const { return id_; }
    TimeOffset get_delivery_interval() const { return di_; }

private:
    ElementID id_;
    TimeOffset di_;
};

#endif