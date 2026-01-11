//
// Created by pawel on 04.01.2026.
//

#ifndef PACKAGE_HPP
#define PACKAGE_HPP
#include "types.hpp"
#include <set>

class Package {
  public:
    Package();
    Package(ElementID id);s
    //przenoszenie
    Package(Package &&other) noexcept;
    Package &operator=(Package &&other) noexcept;
    ~Package();
    //kopiowanie
    Package(const Package&) = delete;
    Package& operator=(const Package&) = delete;

    ElementID get_id() const { return id_; }

   private:
     ElementID id_;
     static const ElementID BLANK_ID = 0;
     static std::set<ElementID> assigned_IDs;
     static std::set<ElementID> freed_IDs;
};


#endif //PACKAGE_HPP
