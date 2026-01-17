//
//lukasz 17.01.26
//
#include "nodes.hpp"
void Ramp::delivergoods(Time t) {
    if (t % di == 1) {
        push_package(Package());
    }
}
//
//
//
