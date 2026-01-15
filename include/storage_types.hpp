//
// Created by pawel on 15.01.2026.
//

#ifndef STORAGE_TYPES_HPP
#define STORAGE_TYPES_HPP

#include "package.hpp"
#include <iostream>
#include <list>

enum class PackageQueueType {
    FIFO,
    LIFO
};

class IPackageStockpile {
  public:
    using const_iterator =  std::list<Package>::const_iterator;

    virtual void push(Package&& package) = 0;

    //metody dostepu
    virtual bool empty() const = 0;
    virtual std::size_t size() const = 0;

    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;

    virtual ~IPackageStockpile() = default;
};

class IPackageQueue : public IPackageStockpile {
public:
    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type() const = 0;
    virtual ~IPackageQueue() = default;
};

class PackageQueue : public IPackageQueue {
  public:

    PackageQueue(PackageQueueType type) : type_(type) {}

    void push(Package&& package) override;
    PackageQueueType get_queue_type() const override{return type_;};
   const_iterator begin() const override{
     return queue_.begin();
   };
   const_iterator end() const override{
     return queue_.end();
   };
   const_iterator cbegin() const override{
     return queue_.cbegin();
   };
   const_iterator cend() const override{
     return queue_.cend();
   };

   bool empty() const override{
	return queue_.empty();}

   std::size_t size() const override{
       return queue_.size();
     }
   Package pop() override;

  private:
   std::list<Package> queue_;
   PackageQueueType type_;
};
#endif //STORAGE_TYPES_HPP
