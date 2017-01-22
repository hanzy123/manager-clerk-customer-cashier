#include "manager.h"
#include "common.h"
#include "utils.h"

#include <thread>


void Manager::operator()(int totalConesNeeded) {
  int numApproved = 0;
  int numInspected = 0;

  while (numApproved < totalConesNeeded) {
    inspection.requested.wait();
    numInspected++;
    inspection.passed = randomPassed();
    atomicCout(cout_mx, "Manager is inspecing ...\n");
    if (inspection.passed) {
      atomicCout(cout_mx, "Manager PASSED one cone\n");
      ++numApproved;
    } else {
      atomicCout(cout_mx, "Manager FAILED one cone\n");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    inspection.finished.notify();
  }
  atomicCout(cout_mx, "Manager DONE\n");
}
