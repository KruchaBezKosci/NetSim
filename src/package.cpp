//
// Created by pawel on 04.01.2026.
//
#include "package.hpp"

std::set<ElementID> Package::assigned_IDs;
std::set<ElementID> Package::freed_IDs;


Package::Package(){
if(!freed_IDs.empty()){
  id_= *freed_IDs.begin();
  freed_IDs.erase(freed_IDs.begin());
} else {
  if (assigned_IDs.empty()) {
    id_ = 1; //zwociu pierwsze id
  } else {
    // zwraca najwiekszy w secie
    id_ = *assigned_IDs.rbegin() + 1;
  }
}
//musi bcyc zajete
assigned_IDs.insert(id_);
};
Package::Package(ElementID id): id_(id){
  assigned_IDs.insert(id);
  if(freed_IDs.count(id_)){
    freed_IDs.erase(id_);
  }
}

Package::Package(Package&&other) noexcept: id_(other.id_){
  other.id_ = BLANK_ID;
}
// Operator przypisania przenoszącego (Move Assignment)
Package& Package::operator=(Package&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    if (id_ != BLANK_ID) {
        assigned_IDs.erase(id_);
        freed_IDs.insert(id_);
    }
    id_ = other.id_;
    other.id_ = BLANK_ID;
    return *this;
}

Package::~Package() {
//tylko jak wazne id
    if (id_ != BLANK_ID) {
        assigned_IDs.erase(id_);
        freed_IDs.insert(id_);
    }
}


