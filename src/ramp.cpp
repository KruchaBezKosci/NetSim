//
//lukasz 17.01.26
//
#include "ramp.hpp"

void Ramp::deliver_goods(Time t) {
    // Dostawa w t=1, t=1+di, t=1+2di itd.
    if ((t - 1) % di_ == 0) {
        push_package(Package());
    }
}