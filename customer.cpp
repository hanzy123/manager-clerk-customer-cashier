#include "common.h"
#include "utils.h"
#include "customer.h"
#include "clerk.h"

#include <thread>

Customer::Customer(int id_) : id(id_) {}

void Customer::operator()(int numCones) {
  atomicCout(cout_mx, "Customer ", id, " is buying ", numCones, " cones ...\n");

  Semaphore clerkDone;

  std::vector<ThreadRAII> clerk_threads;
  for (int i = 0; i < numCones; ++i) {
		ThreadRAII t(std::thread(Clerk(i), std::ref(clerkDone)),
		ThreadRAII::DtorAction::join);
		clerk_threads.emplace_back(std::move(t));
  }

  for (int i = 0; i < numCones; ++i) {
    clerkDone.wait();
  }

  atomicCout(cout_mx, "Customer ", id, " walking to cashier...\n");

  line.lock.wait();
  int place = line.number++;
	line.placeToCustomerId.push_back(this->id);
  line.lock.notify();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  line.requested.notify();
  line.customer[place]->wait();

  atomicCout(cout_mx, "Customer ", id, " 's shopping DONE!\n");

}
