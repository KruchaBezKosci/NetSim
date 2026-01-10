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

Package::~Package() {
//gdy paczka nie została przenisiona gdzies
    assigned_IDs.erase(id_);
    freed_IDs.insert(id_);
}



