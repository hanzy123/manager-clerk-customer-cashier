#include "clerk.h"
#include "common.h"
#include "utils.h"

#include <thread>
#include <mutex>

Clerk::Clerk(int id_) : id(id_) {}

void Clerk::operator()(Semaphore& clerkDone) {
  bool passed = false;
  while (not passed) {
    atomicCout(cout_mx, "Clerk ", id, " is making cones ...\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    {
      std::lock_guard<std::mutex> lk(inspection.mx);
      inspection.requested.notify();
      inspection.finished.wait();
      passed = inspection.passed;

      if (passed) {
        atomicCout(cout_mx, "Clerk ", id, "'s cone PASSED\n");
      } else {
        atomicCout(cout_mx, "Clerk ", id, "'s cone FAILED\n");
      }
    }
  }
  clerkDone.notify();
  atomicCout(cout_mx, "Clerk ", id, " Done\n");
}
