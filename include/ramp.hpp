//
// lukasz 17.01.26
//

#include <memory>
#include <optional>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include "types.hxx"
#include "package.hxx"
#include "storage_types.hxx"

class Ramp : public PackageSender {
public:
    Ramp(ElementID id, TimeOffset di) : PackageSender(), id(id), di(di) {}
    void deliver_goods(Time t);
    ElementID getid() const { return id; }
    TimeOffset get_deliveryinterval() const { return di; }
private:
    ElementID id;
    TimeOffset di;
};
