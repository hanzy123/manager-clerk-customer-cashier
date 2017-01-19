#include "cashier.h"
#include "common.h"
#include "utils.h"

#include <mutex>

void Cashier::operator()() {
  for (int i = 0; i < num_customer; ++i) {
    line.requested.wait();
    atomicCout(cout_mx, "Cashier checkout customer ",
               line.placeToCustomerId[i], " ...\n");
    line.customer[i]->notify();
    atomicCout(cout_mx, "Cashier DONE customer ",
               line.placeToCustomerId[i], "!\n");
  }
}
