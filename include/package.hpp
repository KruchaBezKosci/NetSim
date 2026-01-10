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
    ~Package();
    ElementID get_id() const { return id_; }

   private:
     ElementID id_;
     static std::set<ElementID> assigned_IDs;
     static std::set<ElementID> freed_IDs;
};


#endif //PACKAGE_HPP
