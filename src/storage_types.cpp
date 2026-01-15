//
// Created by pawel on 15.01.2026.
//
#include "storage_types.hpp"
#include "package.hpp"

void PackageQueue::push(Package&& package){
    queue_.emplace_back(std::move(package));
};

Package PackageQueue::pop(){
  Package p;
    if(type_ == PackageQueueType::FIFO){
        p = std::move(queue_.front());
        queue_.pop_front();
    } else{
      p = std::move(queue_.back());
      queue_.pop_back();
    }
  return p;
};

