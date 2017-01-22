#include "common.h"
#include "utils.h"

#include <memory>


Line::Line(int number_, int requested_, int customer_, int lock_) :
    number(number_),
    requested(requested_),
    //customer(num_customer, std::unique_ptr<Semaphore>(new Semaphore(customer_))),
    lock(lock_) {
  for (int i = 0; i < num_customer; ++i) {
    std::unique_ptr<Semaphore> p(new Semaphore(customer_));
    customer.push_back(std::move(p));
  }
    }


Inspection::Inspection(bool passed_,
                      int requested_count,
                      int finished_count) :
    passed(passed_),
    requested(requested_count),
    finished(finished_count) {}

Inspection::Inspection() : passed(false),
                          requested(0),
                          finished(0) {}
