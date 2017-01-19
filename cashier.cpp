#include "cashier.h"
#include "common.h"
#include "utils.h"

#include <mutex>

void Cashier::operator()() {
  for (int i = 0; i < num_customer; ++i) {
    line.requested.wait();
    atomicCout(cout_mx, "Cashier checkout ", i, "th customer...");
    line.customer[i]->notify();
    atomicCout(cout_mx, "Cashier DONE ", i, "th customer!");
  }
}
