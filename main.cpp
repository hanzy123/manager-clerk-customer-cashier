#include "common.h"
#include "manager.h"
#include "clerk.h"
#include "customer.h"
#include "cashier.h"

#include <thread>


const int num_customer =10;
Line line(0, 0, 0, 1);
std::mutex cout_mx;
Inspection inspection;

int main() {
  int totalCones = 0;

  ThreadRAII customer_threads[num_customer];
  for (int i = 0; i < num_customer; ++i) {
    int numCones = getRandomInt(1, 4);
		
		customer_threads[i] = ThreadRAII(std::thread(Customer(i), numCones), ThreadRAII::DtorAction::join);
		
    totalCones += numCones;
  }


  ThreadRAII manager_thread(std::thread(Manager(), totalCones),
                            ThreadRAII::DtorAction::join);


  ThreadRAII cashier_thread(std::thread(Cashier()),
                            ThreadRAII::DtorAction::join);

  return 0;
}
